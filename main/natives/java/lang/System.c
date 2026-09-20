#include "../../../native_methods_service.h"
#include "../../../heap.h"
#include "../../../jerror.h"
#include "../../../class.h"

#include <stdlib.h>
#include <string.h>

static NativeMethodReturnValue_t arraycopy(Thread_t* ctx, Method_t* self, int32_t* args){
    Object_t* src = (Object_t*)args[0];
    int32_t src_pos = args[1];
    Object_t* dst = (Object_t*)args[2];
    int32_t dst_pos = args[3]; 
    int32_t length = args[4];  
    
    if(!src || !dst) return (NativeMethodReturnValue_t){JERR_NULLPOINTER, {0}};

    if(!src->class->flags.is_array || !dst->class->flags.is_array || (src->class->array_type != dst->class->array_type)){
        NativeMethodReturnValue_t retval = {0};
        retval.err = JERR_UNKNOWN;
        return retval;
    }

    int32_t src_length = OBJECT_ARRAY_LENGTH(src);
    int32_t dst_length = OBJECT_ARRAY_LENGTH(dst);

    if(src_pos < 0 || dst_pos < 0 || length < 0 || src_pos + length > src_length || dst_pos + length > dst_length){
        NativeMethodReturnValue_t retval = {0};
        retval.err = JERR_INDEXOOB;
        return retval;       
    }

    void* src_data = OBJECT_ARRAY_ELEMENTS(src, void);
    void* dst_data = OBJECT_ARRAY_ELEMENTS(dst, void);

    size_t element_size = heap_array_type_size(src->class->array_type);
    void* src_data_offseted = (char*)src_data + (src_pos * element_size);
    void* dst_data_offseted = (char*)dst_data + (dst_pos * element_size);

    memmove(dst_data_offseted, src_data_offseted, length * heap_array_type_size(dst->class->array_type));

    return (NativeMethodReturnValue_t){JERR_OK, {0}};
}

static NativeMethodReturnValue_t currentTimeMillis(Thread_t* ctx, Method_t* self, int32_t* args){
    NativeMethodReturnValue_t retval = {0};
    retval.err = JERR_OK;
    *(int64_t*)retval.value = 0;

    return retval;
}

static NativeMethodReturnValue_t gc(Thread_t* ctx, Method_t* self, int32_t* args){
    heap_gc_start();
    return (NativeMethodReturnValue_t){JERR_OK, {0}};
}

static NativeMethodReturnValue_t identityHashCode(Thread_t* ctx, Method_t* self, int32_t* args){
    NativeMethodReturnValue_t retval = {0};
    retval.err = JERR_OK;
    *(int32_t*)retval.value = ((Object_t*)args[0])->ident;

    return retval;
}
   
static NativeMethodReturnValue_t vm_exit(Thread_t* ctx,  Method_t* self, int32_t* args){
    //TODO: change it to proper exit
    exit(args[0]);
}

NativeClass_t java_lang_System = {
    .name = "java/lang/System",
    .methods_count = 5,
    .methods = (NativeMethodDescriptor_t[]){
        {"arraycopy@(Ljava/lang/Object;ILjava/lang/Object;II)V", arraycopy},
        {"currentTimeMillis@()J",currentTimeMillis},
        {"gc@()V",gc},
        {"identityHashCode@(Ljava/lang/Object;)I", identityHashCode},
        {"exit@(I)V", vm_exit},
    },
};