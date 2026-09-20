#pragma once

#include <stdint.h>

#include "list.h"
#include "monitor.h"

#define SHADOW_CLEAR_REF(bitmap, idx)  ((bitmap)[(idx) >> 5] &= ~(1U << ((idx) & 31)))
#define SHADOW_SET_REF(bitmap, idx)    ((bitmap)[(idx) >> 5] |= (1U << ((idx) & 31)))
#define SHADOW_GET_REF(bitmap, idx)    (((bitmap)[(idx) >> 5] & (1U << ((idx) & 31))) ? 1 : 0)

typedef struct InterpreterFrame_t InterpreterFrame_t;
typedef struct Method_t Method_t;
typedef struct Class_t Class_t;
typedef struct InterpreterFrame_t{
    size_t size; //required because of arena
    struct list_head held_monitors; //Inside of Frame_t because of java semantics for exception unwind

    Method_t* method;
    uint8_t* pc;
    uint32_t sp;
    
    int32_t* stack;
    int32_t* locals;

    uint32_t* shadow_locals;
    uint32_t* shadow_stack;

    InterpreterFrame_t* prev;
}InterpreterFrame_t;

void interpreter_init();
InterpreterFrame_t* interpreter_frame_push(Thread_t* thread, Method_t* method);
InterpreterFrame_t* interpreter_frame_pop(Thread_t* ctx);
InterpreterFrame_t* interpreter_frame_get(Thread_t* ctx);

//Special function to invoke java method on top of current java thread
Error_t interpreter_execute(Thread_t* thread);