#include "../../../native_methods_service.h"
#include "../../../heap.h"
#include "../../../class.h"
#include "../../../interpreter.h"
#include "../../../jerror.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <assert.h>
#include <unistd.h>
#include <wchar.h>
#include <sys/select.h>

#define WRITE_CHUNK_SIZE 512
static NativeMethodReturnValue_t ns_open(Thread_t* ctx, Method_t* method, int32_t* args){
    assert(0 && "Broken now (remade for new java.lang.String)");
    
    Object_t* path = (Object_t*)args[0];
    //int32_t flags = args[1];

    if(path == NULL){
        NativeMethodReturnValue_t retval = {0};
        retval.err = JERR_NULLPOINTER;
        return retval;            
    }

    void* storage = NULL;
    //assert(heap_class_object_get_fields(path, &storage) == JERR_OK);
    
    Field_t *value = NULL, *count = NULL, *offset = NULL;
    assert((value = class_find_field_cstr(path->class, "value@[C")));
    assert((count = class_find_field_cstr(path->class, "count@I")));
    assert((offset = class_find_field_cstr(path->class, "offset@I")));

    Object_t* char_array = *(Object_t**)&OBJECT_FIELDS(path)[value->offset];

    int16_t* chars = OBJECT_ARRAY_ELEMENTS(char_array, int16_t);

    int32_t real_length = *(int32_t*)(storage + count->offset) - *(int32_t*)(storage + offset->offset);
    int16_t* start = chars + *(int32_t*)(storage + offset->offset);

    char path_str[real_length + 1];
    //wcstombs(path_str, (wchar_t*)start, real_length > sizeof(path_str) ? real_length : sizeof(path_str));

    //TODO: UTF8
    for(unsigned i = 0; i < real_length; i++){
        path_str[i] = start[i];
    }

    path_str[real_length] = '\0';

    NativeMethodReturnValue_t return_value = {0};
    return_value.err = JERR_OK;
    *(int*)return_value.value = open(path_str, O_WRONLY | O_CREAT, 0644);

    return return_value;
}

static NativeMethodReturnValue_t ns_close(Thread_t* ctx, Method_t* method, int32_t* args){
    Object_t* self = (Object_t*)args[0];

    Field_t *fd = NULL;
    assert((fd = class_find_field_cstr(self->class, "fd@I")));

    close(*(int32_t*)&OBJECT_FIELDS(self)[fd->offset]);

    return (NativeMethodReturnValue_t){JERR_OK, {0}};
}

static NativeMethodReturnValue_t ns_flush(Thread_t* ctx, Method_t* method, int32_t* args){
    Object_t* self = (Object_t*)args[0];

    Field_t *fd = NULL;
    assert((fd = class_find_field_cstr(self->class, "fd@I")));

    fsync(*(int32_t*)&OBJECT_FIELDS(self)[fd->offset]);

    return (NativeMethodReturnValue_t){JERR_OK, {0}};
}

static NativeMethodReturnValue_t ns_write(Thread_t* ctx, Method_t* method, int32_t* args){
    NativeMethodReturnValue_t retval = {0};

    int32_t fd = args[0];
    int32_t remaining = OBJECT_ARRAY_LENGTH(((Object_t*)args[1]));
    int32_t written = 0;

    while(remaining > 0){
        int write_len = remaining > WRITE_CHUNK_SIZE ? WRITE_CHUNK_SIZE : remaining;

        uint8_t* bytes = OBJECT_ARRAY_ELEMENTS(((Object_t*)args[1]), uint8_t);
        int32_t really_written = 0;
        if((really_written = write(fd, bytes + written, write_len)) < 0){
            assert(0 && "TODO: IOException throw");
        }

        remaining -= really_written;
        written += really_written;

    }

    return retval;
}

NativeClass_t java_io_NativeOutputStream = {
    .name = "java/io/NativeOutputStream",
    .methods_count = 4,
    .methods = (NativeMethodDescriptor_t[]){
        {"open@(Ljava/lang/String;I)I", ns_open},
        {"close@()V",ns_close},
        {"flush@()V",ns_flush},
        {"write_fd@(I[B)V", ns_write},
    },
};