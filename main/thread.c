#include "thread.h"
#include "config.h"
#include "heap.h"
#include "interpreter.h"
#include "jerror.h"
#include "list.h"
#include "class.h"

#include <string.h>
#include <assert.h>

static LIST_HEAD(s_active_threads);
static LIST_HEAD(s_sleep_threads);

static Thread_t* s_current_thread = NULL;

void thread_scheduler_add(Thread_t* thread){
    list_del_init(&thread->list);
    list_add_tail(&thread->list, &s_active_threads);
}

Thread_t* thread_run(Object_t* jlThread, Method_t* method, int32_t* args){
    Thread_t* new_thread = calloc(1, sizeof(*new_thread));
    assert(new_thread);

    INIT_LIST_HEAD(&new_thread->list);
    INIT_LIST_HEAD(&new_thread->sleep_list);
    INIT_LIST_HEAD(&new_thread->gc_list);
    INIT_LIST_HEAD(&new_thread->joiners);


    heap_gc_handle_register(&new_thread->jlThread);
    heap_gc_handle_register(&new_thread->pending_exception);
    JeNIHANDLE_OBJECT(new_thread->jlThread) = jlThread;


    new_thread->state = THREAD_RUNNING;
    new_thread->priority = THREAD_DEFAULT_PRIORITY;
    new_thread->bytecode_quota = THREAD_LOWEST_QUOTA * new_thread->priority;

    new_thread->wakeup_on = 0;
    new_thread->wake_recursion = 0;
    new_thread->frame = NULL;
    bumper_create_from(&new_thread->arena, new_thread->stackbuf, sizeof(new_thread->stackbuf));

    InterpreterFrame_t* start = interpreter_frame_push(new_thread, method);
    assert(start);

    memcpy(start->locals, args, method->args_slots * sizeof(int32_t)); //Copy arguments

    heap_gc_thread_register(new_thread); //Register thread inside GC
    thread_scheduler_add(new_thread);

    return new_thread;
}

void thread_exit(Thread_t* thread){
    list_del(&thread->list);
    list_del(&thread->sleep_list);
    list_del(&thread->gc_list);

    Thread_t *to_awake = NULL, *tmp = NULL;
    list_for_each_entry_safe(to_awake, tmp, &thread->joiners, list){
        list_del_init(&to_awake->list);
        list_add_tail(&to_awake->list, &s_active_threads);
    }

    heap_gc_handle_unregister(&thread->jlThread);
    heap_gc_handle_unregister(&thread->pending_exception);

    s_current_thread = NULL;
    free(thread);
}

Error_t thread_schedule(){
    Error_t err = JERR_OK;
    
schedule_procced:
    while(!list_empty(&s_active_threads) || !list_empty(&s_sleep_threads)){
        if(!list_empty(&s_active_threads)){
            Thread_t* next = list_entry(s_active_threads.next, Thread_t, list);
            s_current_thread = next;
            
            thread_scheduler_add(next); //It will remove it and add back to tail
            FAIL_SET_JUMP((err = interpreter_execute(next)) == JERR_OK, err, err, exit);
        }

        Thread_t *sleeper = NULL, *tmp = NULL;
        list_for_each_entry_safe(sleeper, tmp, &s_sleep_threads, list){
            assert(0 && "TODO:");
        }
    }

exit:
    switch(err){
        case JERR_SCHEDULE: goto schedule_procced;
        default: return err;
    }
}

Thread_t* thread_self(){
    return s_current_thread;
}