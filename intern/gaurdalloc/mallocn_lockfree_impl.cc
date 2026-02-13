

#include <cstdarg>
#include <cstdint>

#include "malloc_inline.hh"
#include "mallocn_intern_function_pointers.hh"
#include "mallocn_lockfree.hh"

#define MEM_MIN_CPP_ALIGNMENT                                                                                          \
    (__STDCPP_DEFAULT_NEW_ALIGNMENT__ < alignof(void*) ? __STDCPP_DEFAULT_NEW_ALIGNMENT__ : alignof(void*))

namespace vektor
{

static void print_error(const char* message,
                        ...)
{
    va_list str_format_args;
    va_start(str_format_args, message);
    print_error(message, str_format_args);
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

void* MEM_new_uninitialized_aligned(size_t      len,
                                    size_t      alignment,
                                    const char* str)
{
    return mem_guarded::internal::mem_mallocN_aligned_ex(len, alignment, str,
                                                         mem_guarded::internal::DestructorType::Trivial);
}

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
    void* ptr = MEM_new_uninitialized_aligned(r_bytes_num, alignment, str);
    return ptr;
}

} // namespace vektor
