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
#include "list.h"
#include "jerror.h"

typedef struct MonitorLockWord_t{
    union{
        uint32_t word;
        struct{
            unsigned int tag:2; //Will also be used to safe some space for GC. This is a hack
            
            union{
                struct{
                    unsigned int thread_id:10;
                    unsigned int recursion:20;
                }thin_lock;

                unsigned int fatlock:30; //It is a pointer to monitor
            };
        };
    };
}MonitorLockWord_t;

typedef struct Thread_t Thread_t;
typedef struct Object_t Object_t;
typedef struct FatLock_t{
    //struct list_head list;
    struct list_head enter_set; //list of threads that awaiting the objects unlocking
    struct list_head wait_set; //Object.wait()

    Object_t* owner_object;
    Thread_t* owner;
    uint32_t recursion;
}FatLock_t;