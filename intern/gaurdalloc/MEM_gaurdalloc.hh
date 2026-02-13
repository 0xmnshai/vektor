#include <stddef.h>
#include <type_traits>

#include "malloc_function_pointers.hh"
#include "mallocn_lockfree.hh"

void* (*MEM_new_array_zeroed_aligned)(size_t      len,
                                      size_t      size,
                                      size_t      alignment,
                                      const char* str,
                                      size_t&     actual_size) = vektor::mem_lockfree_malloc_arrayN_aligned;

template <typename T>
inline T* MEM_new_zeroed(const char* allocation_name)
{
    static_assert(std::is_trivial<T>::value, "For non-trivial types, MEM_new must be used.");
    size_t actual_size;
    return static_cast<T*>(MEM_new_array_zeroed_aligned(1, sizeof(T), alignof(T), allocation_name, actual_size));
}

template <typename T>
inline void MEM_delete(const T* ptr)
{
    static_assert(!std::is_void<T>::value,
                  "MEM_delete on a void pointer is not possible, `static_cast` it to the correct type");
    if (ptr == nullptr)
    {
        return;
    }
    const void* complete_ptr = [ptr]()
    {
        if (std::is_polymorphic<T>::value)
        {
            return static_cast<const void*>(ptr);
        }
        else
        {
            return static_cast<const void*>(ptr);
        }
    }();
    if (!std::is_trivially_destructible<T>::value)
    {
        ptr->~T();
    }
    vektor::mem_guarded::internal::mem_freeN_ex(const_cast<void*>(complete_ptr),
                                                vektor::mem_guarded::internal::DestructorType::NonTrivial);
}