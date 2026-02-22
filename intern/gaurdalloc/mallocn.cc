#include "malloc_function_pointers.hh"
#include "mallocn_intern.hh"
#include "mallocn_lockfree.hh"

void* MEM_new_uninitialized_aligned(size_t      len,
                                    size_t      alignment,
                                    const char* str)
{
    return vektor::mem_guarded::internal::mem_mallocN_aligned_ex(
        len, alignment, str, vektor::mem_guarded::internal::DestructorType::Trivial);
}

void* (*vektor::mem_guarded::internal::mem_mallocN)(size_t      len,
                                                    const char* str) = MEM_lockfree_mallocN;

void* (*vektor::mem_guarded::internal::mem_mallocN_aligned_ex)(
    size_t                                        len,
    size_t                                        alignment,
    const char*                                   str,
    vektor::mem_guarded::internal::DestructorType destructor_type) = MEM_lockfree_mallocN_aligned;

void (*vektor::mem_guarded::internal::mem_freeN_ex)(void*                                         ptr,
                                                    vektor::mem_guarded::internal::DestructorType destructor_type) =
    MEM_lockfree_freeN;

void (*MEM_set_error_callback)(void (*func)(const char*)) = MEM_lockfree_set_error_callback;

void* MEM_lockfree_malloc_arrayN_aligned(const size_t len,
                                         const size_t size,
                                         const size_t alignment,
                                         const char*  str)
{
    size_t bytes_num;
    return vektor::mem_lockfree_malloc_arrayN_aligned(len, size, alignment, str, bytes_num);
}

void MEM_delete_void(void* vmemh)
{
    vektor::mem_guarded::internal::mem_freeN_ex(vmemh, vektor::mem_guarded::internal::DestructorType::Trivial);
}
