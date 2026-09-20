/*
JEspressoVM - project to bring java bytecode execution to esp32 (and others)

Copyright (C) 2026  Vladislav Potrashkov

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "list.h"

#include <sys/types.h>
#include <assert.h>

#define GC_MARK_SENTINEL (void*)0xFFFFFFFF

typedef enum ValueType_t ValueType_t;
typedef struct Class_t Class_t;
typedef struct Thread_t Thread_t;

typedef struct Object_t{
    struct list_head list; //Live object list(GC simplification currently)

    Class_t* class;
    uint32_t ident;
    void* forward;

    uint32_t mark_word; //Also the lock_word
    uint8_t fields[];
}Object_t;

typedef struct{
    struct list_head list; //GC root list
    Object_t* object;
}ObjectJeNIHandle_t;

#define JeNIHANDLE_OBJECT(handle) (handle).object

#define JeNIHANDLE(name) __attribute__((cleanup(heap_gc_handle_unregister))) ObjectJeNIHandle_t (name) = {0};\
                         heap_gc_handle_register(&(name))

#define OBJECT_CLASS(object) (object)->class

//Thoose macros output UNSTABLE pointers. They might and WILL change after every GC. DO NOT STORE THEM
#define OBJECT_FIELDS(object) (object)->fields

#define OBJECT_ARRAY_LENGTH(object) (*(uint32_t*)(&object->fields[0]))
#define OBJECT_ARRAY_SIZE(object) (sizeof(Object_t) + (OBJECT_ARRAY_LENGTH((object)) * heap_array_type_size((object)->class->array_type)) + sizeof(uint32_t))
#define OBJECT_ARRAY_ELEMENTS(object, type) ((type*)(object->fields + sizeof(uint32_t)))
//====================================================================================================

void heap_init();
int heap_array_type_size(ValueType_t type);

void heap_gc_thread_register(Thread_t* thread);
void heap_gc_thread_unregister(Thread_t* thread);

void heap_gc_start();

uint32_t heap_object_get_hashcode(Object_t* object);

Object_t* heap_alloc_object(Class_t* class);
Object_t* heap_alloc_array(Class_t* class, uint32_t length);

void heap_gc_handle_register(ObjectJeNIHandle_t* handle);
void heap_gc_handle_unregister(ObjectJeNIHandle_t* handle);

/*void heap_object_read(Object_t* object, Field_t* field, void* output);
void heap_object_write(Object_t* object, Field_t*, void* input);

void heap_array_read(Object_t* array, size_t index, void* output);
void heap_array_write(Object_t* array, size_t index, void* input);
*/