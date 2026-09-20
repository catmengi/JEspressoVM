#pragma once

#include "bumper.h"
#include "config.h"
#include "heap.h"
#include "list.h"
#include "interpreter.h"
#include "monitor.h"

typedef enum{ //Other states not required now, since monitor will take owner ship of thread(remove from sched)
    THREAD_RUNNING,
    THREAD_TIMEDWAIT,
    THREAD_SLEEP,
}ThreadState_t;

typedef struct Thread_t{
    struct list_head list; //List used for scheduling / monitor operations / safepoint waiting / waiting for thread exit (join)
    struct list_head sleep_list; //Second list, for timedWait and sleep()

    struct list_head joiners; //List of threads that want to join us
    struct list_head gc_list; //required for GC scanning

    ThreadState_t state;
    ObjectJeNIHandle_t jlThread; //java.lang.Thread. Need for GC
    ObjectJeNIHandle_t pending_exception;
    
    //MonitorLockWord_t* pending_enter; //Scheduler will check it for non-NULL, if so: tries to enter the monitor

    unsigned priority;
    unsigned bytecode_quota;

    //TODO: refactor frame system!
    bump_allocator_t arena; 
    InterpreterFrame_t* frame; //Top frame
    char stackbuf[THREAD_STACK_SIZE]; //Buffer that arena uses
    //=============================

    unsigned wake_recursion; //if 0 then thread is NOT in waiting state
    uint64_t wakeup_on; //Time when thread must wakeup (if sleeping!)
}Thread_t;

Thread_t* thread_run(Object_t* jlThread, Method_t* method, int32_t* args);
Thread_t* thread_self();

void thread_exit(Thread_t* thread);
void thread_scheduler_add(Thread_t* thread); //Removes thread from whatever list it was (by thread.list) and adds it to scheduler list

Error_t thread_schedule();