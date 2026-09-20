#pragma once

#include "class.h"
#include "heap.h"
#include "list.h"
#include <stdint.h>

void      jstringpool_init(void);
Object_t* jstringpool_get(int32_t name_id);

/* GC integration. Call from gc_scan() and gc_patch(). */
void gc_scan_stringpool(struct list_head* output_list);
void gc_patch_stringpool(void);