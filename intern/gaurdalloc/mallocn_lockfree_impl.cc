#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "malloc_inline.hh"
#include "mallocn_lockfree.hh"

#ifndef __STDCPP_DEFAULT_NEW_ALIGNMENT__
#define __STDCPP_DEFAULT_NEW_ALIGNMENT__ 16
#endif

#define MEM_MIN_CPP_ALIGNMENT                                                                                          \
    (__STDCPP_DEFAULT_NEW_ALIGNMENT__ < alignof(void*) ? __STDCPP_DEFAULT_NEW_ALIGNMENT__ : alignof(void*))

using namespace vektor;

static void print_error(const char* message,
                        ...)
{
    va_list str_format_args;
    va_start(str_format_args, message);
    vfprintf(stderr, message, str_format_args);
    va_end(str_format_args);
}

static bool malloc_debug_memset            = false;

static void (*error_callback)(const char*) = nullptr;

void MEM_lockfree_set_error_callback(void (*func)(const char*))
{
    error_callback = func;
}

bool MEM_lockfree_consistency_check()
{
    return true;
}

void MEM_lockfree_set_memory_debug()
{
    malloc_debug_memset = true;
}

size_t MEM_lockfree_get_memory_in_use()
{
    return memory_usage_current();
}

uint32_t MEM_lockfree_get_memory_blocks_in_use()
{
    return uint32_t(memory_usage_block_num());
}

void* MEM_lockfree_mallocN(size_t      len,
                           const char* str)
{
    void* ptr = malloc(len);
    if (!ptr && len > 0)
    {
        if (error_callback)
            error_callback("Malloc failed");
        else
            fprintf(stderr, "Malloc failed\n");
        abort();
    }
    return ptr;
}

void MEM_lockfree_freeN(void*                                         ptr,
                        vektor::mem_guarded::internal::DestructorType destructor_type)
{
    if (ptr)
    {
        free(ptr);
    }
}

void* MEM_lockfree_mallocN_aligned(size_t                                        len,
                                   size_t                                        alignment,
                                   const char*                                   str,
                                   vektor::mem_guarded::internal::DestructorType destructor_type)
{
    if (alignment < sizeof(void*))
    {
        alignment = sizeof(void*);
    }

    void*  ptr         = nullptr;

    size_t aligned_len = (len + alignment - 1) & ~(alignment - 1);

#ifdef _WIN32
    ptr = _aligned_malloc(len, alignment);
#else
    ptr = aligned_alloc(alignment, aligned_len);
#endif

    if (!ptr && len > 0)
    {
        if (error_callback)
            error_callback("Aligned malloc failed");
        else
            fprintf(stderr, "Aligned malloc failed\n");
        abort();
    }
    return ptr;
}

void* MEM_lockfree_callocN(size_t      len,
                           const char* str)
{
    void* ptr = calloc(1, len);
    if (!ptr && len > 0)
    {
        if (error_callback)
            error_callback("Calloc failed");
        abort();
    }
    return ptr;
}

void* MEM_lockfree_reallocN_id(void*       vmemh,
                               size_t      len,
                               const char* str)
{
    void* ptr = realloc(vmemh, len);
    if (!ptr && len > 0)
    {
        if (error_callback)
            error_callback("Realloc failed");
        abort();
    }
    return ptr;
}

void* MEM_lockfree_recallocN_id(void*       vmemh,
                                size_t      len,
                                const char* str)
{

    return MEM_lockfree_reallocN_id(vmemh, len, str);
}

void* MEM_lockfree_dupallocN(const void* vmemh)
{

    print_error("MEM_lockfree_dupallocN not supported in this lockfree backend without size tracking\n");
    return nullptr;
}

namespace vektor
{

void* mem_lockfree_malloc_arrayN_aligned(const size_t len,
                                         const size_t size,
                                         const size_t alignment,
                                         const char*  str,
                                         size_t&      r_bytes_num)
{
    if (UNLIKELY(!MEM_size_safe_multiply(len, size, &r_bytes_num)))
    {
        print_error("Calloc array aborted due to integer overflow: "
                    "len=" SIZET_FORMAT "x" SIZET_FORMAT " in %s, total " SIZET_FORMAT "\n",
                    SIZET_ARG(len), SIZET_ARG(size), str, memory_usage_current());
        abort();
        return nullptr;
    }
    if (alignment <= MEM_MIN_CPP_ALIGNMENT)
    {
        return mem_guarded::internal::mem_mallocN(r_bytes_num, str);
    }
    void* ptr = mem_guarded::internal::mem_mallocN_aligned_ex(r_bytes_num, alignment, str,
                                                              mem_guarded::internal::DestructorType::Trivial);
    return ptr;
}

} // namespace vektor
