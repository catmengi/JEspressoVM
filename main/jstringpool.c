#include "jstringpool.h"
#include "bumper.h"
#include "converter.h"
#include "list.h"
#include "memman.h"
#include "stringpool.h"

#include <assert.h>
#include <string.h>

//This module is fully vibecoded by deepseek using classtable.c and stringpool.c as base and reference
//If you feel offended by it, idk, do something

typedef struct {
    struct list_head list;
    Object_t*        items[STRINGPOOL_ENTRY_ITEMS_COUNT];
} JStringpoolEntry_t;

static bump_allocator_t* s_arena = NULL;
static struct list_head  s_entry_list  = {0};
static size_t            s_entry_count = 0;

static JStringpoolEntry_t* insert_entry(void) {
    JStringpoolEntry_t* entry = bumper_calloc(s_arena, 1, sizeof(*entry));
    if (!entry) return NULL;

    INIT_LIST_HEAD(&entry->list);
    list_add_tail(&entry->list, &s_entry_list);
    s_entry_count++;
    return entry;
}

void jstringpool_init(void) {
    assert((s_arena = memman_get(VM_PERMA_ARENA_ID)));
    INIT_LIST_HEAD(&s_entry_list);
    s_entry_count = 0;
    assert(insert_entry());
}

static Object_t** find_slot(int32_t name_id) {
    uint32_t bucket = (uint32_t)name_id / STRINGPOOL_ENTRY_ITEMS_COUNT;
    uint32_t slot   = (uint32_t)name_id % STRINGPOOL_ENTRY_ITEMS_COUNT;

    if (bucket >= s_entry_count) return NULL;

    struct list_head* cur = s_entry_list.next;
    for (uint32_t i = 0; i < bucket; i++, cur = cur->next) {}
    JStringpoolEntry_t* entry = list_entry(cur, JStringpoolEntry_t, list);
    return &entry->items[slot];
}

static Object_t** ensure_slot(int32_t name_id) {
    uint32_t bucket = (uint32_t)name_id / STRINGPOOL_ENTRY_ITEMS_COUNT;
    while (s_entry_count <= bucket) {
        if (!insert_entry()) return NULL;
    }
    return find_slot(name_id);
}

/*
 * Return the interned java.lang.String for name_id, constructing it on
 * first use. Returns NULL on any failure.
 *
 * GC safety: `chars` and `str` are held in JeNIHANDLE slots so they survive
 * heap_alloc_* calls that may trigger collection. Access through
 * JeNIHANDLE_OBJECT(...) after any allocation, never through the raw
 * local, because GC may have moved the target.
 */
Object_t* jstringpool_get(int32_t name_id) {
    /* Fast path: interned. Not a GC'd heap object — safe to return raw. */
    Object_t** slot = find_slot(name_id);
    if (slot && *slot) return *slot;

    /* Source C string. */
    const char* cstr = stringpool_get(name_id);
    if (!cstr) return NULL;

    size_t cstr_len = strlen(cstr);

    /* UTF-16 length. */
    int u16len = utf8_to_utf16((const utf8_t*)cstr, cstr_len, NULL, 0);
    if (u16len < 0) return NULL;

    /* Load [C. */
    Class_t* char_arr_class = NULL;
    {
        int id = stringpool_add("[C");
        if (id < 0) return NULL;
        if (class_load_bynameid((uint16_t)id, &char_arr_class) != JERR_OK) return NULL;
    }

    /* Load java/lang/String. */
    Class_t* str_class = NULL;
    {
        int id = stringpool_add("java/lang/String");
        if (id < 0) return NULL;
        if (class_load_bynameid((uint16_t)id, &str_class) != JERR_OK) return NULL;
    }

    /* Resolve fields. */
    Field_t* f_chars = class_find_field_cstr(str_class, "string_chars@[C");
    Field_t* f_hash  = class_find_field_cstr(str_class, "hash@I");
    Field_t* f_name  = class_find_field_cstr(str_class, "name_id@I");
    if (!f_chars || !f_hash || !f_name) return NULL;

    /* -------- GC-safe region starts here -------- */
    JeNIHANDLE(chars_h);
    JeNIHANDLE(str_h);
    /* Cleanup handlers run on any return from this function, including
       error paths, unregistering both slots. */

    /* Backing char[] and payload. */
    Object_t* chars = heap_alloc_array(char_arr_class, (size_t)u16len);
    if (!chars) return NULL;
    JeNIHANDLE_OBJECT(chars_h) = chars;
    /* From now on, always use JeNIHANDLE_OBJECT(chars_h), not `chars`. */

    {
        utf16_t* dst = OBJECT_ARRAY_ELEMENTS(JeNIHANDLE_OBJECT(chars_h), utf16_t);
        int written  = utf8_to_utf16((const utf8_t*)cstr, cstr_len, dst, u16len);
        if (written != u16len) return NULL;
        /* No allocation above this line, so dst is still valid. */
    }

    /* String object. May trigger GC; chars_h survives. */
    Object_t* str = heap_alloc_object(str_class);
    if (!str) return NULL;
    JeNIHANDLE_OBJECT(str_h) = str;

    /* Direct field writes. Re-read handles because GC may have moved objects. */
    {
        uint8_t* fields = OBJECT_FIELDS(JeNIHANDLE_OBJECT(str_h));
        *(Object_t**)(fields + f_chars->offset) = JeNIHANDLE_OBJECT(chars_h);
        *(int32_t*) (fields + f_hash->offset)   = 0;
        *(int32_t*) (fields + f_name->offset)   = name_id;
    }

    /* Intern. Slot lives in perma arena, never moves. */
    slot = ensure_slot(name_id);
    if (!slot) return NULL;
    *slot = JeNIHANDLE_OBJECT(str_h);

    return JeNIHANDLE_OBJECT(str_h);
}

/* -------------------- GC integration -------------------- */

void gc_scan_stringpool(struct list_head* output_list) {
    JStringpoolEntry_t* entry = NULL;
    list_for_each_entry(entry, &s_entry_list, list) {
        for (unsigned i = 0; i < STRINGPOOL_ENTRY_ITEMS_COUNT; i++) {
            Object_t* object = entry->items[i];
            if (object && object->forward != GC_MARK_SENTINEL) {
                object->forward = GC_MARK_SENTINEL;
                INIT_LIST_HEAD(&object->list);
                list_add_tail(&object->list, output_list);
            }
        }
    }
}

void gc_patch_stringpool(void) {
    JStringpoolEntry_t* entry = NULL;
    list_for_each_entry(entry, &s_entry_list, list) {
        for (unsigned i = 0; i < STRINGPOOL_ENTRY_ITEMS_COUNT; i++) {
            Object_t* object = entry->items[i];
            if (object && object->forward != GC_MARK_SENTINEL) {
                entry->items[i] = object->forward;
            }
        }
    }
}