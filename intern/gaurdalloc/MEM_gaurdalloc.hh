#pragma once

#include <stddef.h>
#include <new> // IWYU pragma: keep
#include <type_traits>

#include "malloc_function_pointers.hh"
#include "mallocn_lockfree.hh"

#define MEM_CXX_CLASS_ALLOC_FUNCS(_id)                                                                                 \
public:                                                                                                                \
    void* operator new(size_t num_bytes)                                                                               \
    {                                                                                                                  \
        return mem_guarded::internal::mem_mallocN_aligned_ex(num_bytes, __STDCPP_DEFAULT_NEW_ALIGNMENT__, _id,         \
                                                             mem_guarded::internal::DestructorType::NonTrivial);       \
    }                                                                                                                  \
    void* operator new(size_t num_bytes, std::align_val_t alignment)                                                   \
    {                                                                                                                  \
        return mem_guarded::internal::mem_mallocN_aligned_ex(num_bytes, size_t(alignment), _id,                        \
                                                             mem_guarded::internal::DestructorType::NonTrivial);       \
    }                                                                                                                  \
    void operator delete(void* mem)                                                                                    \
    {                                                                                                                  \
        if (mem)                                                                                                       \
        {                                                                                                              \
            mem_guarded::internal::mem_freeN_ex(mem, mem_guarded::internal::DestructorType::NonTrivial);               \
        }                                                                                                              \
    }                                                                                                                  \
    void* operator new[](size_t num_bytes)                                                                             \
    {                                                                                                                  \
        return mem_guarded::internal::mem_mallocN_aligned_ex(num_bytes, __STDCPP_DEFAULT_NEW_ALIGNMENT__, _id "[]",    \
                                                             mem_guarded::internal::DestructorType::NonTrivial);       \
    }                                                                                                                  \
    void* operator new[](size_t num_bytes, std::align_val_t alignment)                                                 \
    {                                                                                                                  \
        return mem_guarded::internal::mem_mallocN_aligned_ex(num_bytes, size_t(alignment), _id "[]",                   \
                                                             mem_guarded::internal::DestructorType::NonTrivial);       \
    }                                                                                                                  \
    void operator delete[](void* mem)                                                                                  \
    {                                                                                                                  \
        if (mem)                                                                                                       \
        {                                                                                                              \
            mem_guarded::internal::mem_freeN_ex(mem, mem_guarded::internal::DestructorType::NonTrivial);               \
        }                                                                                                              \
    }                                                                                                                  \
    void* operator new(size_t /*count*/, void* ptr)                                                                    \
    {                                                                                                                  \
        return ptr;                                                                                                    \
    }

inline void* (*MEM_new_array_zeroed_aligned)(size_t      len,
                                             size_t      size,
                                             size_t      alignment,
                                             const char* str,
                                             size_t&     actual_size) = vektor::mem_lockfree_malloc_arrayN_aligned;

void* MEM_new_array_uninitialized(size_t      len,
                                  size_t      size,
                                  const char* str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                             2)
    ATTR_NONNULL(3);

extern void* (*MEM_new_array_uninitialized_aligned)(size_t      len,
                                                    size_t      size,
                                                    size_t      alignment,
                                                    const char* str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT
    ATTR_ALLOC_SIZE(1,
                    2) ATTR_NONNULL(4);

template <typename T>
inline T* MEM_new_array_uninitialized(const size_t length,
                                      const char*  allocation_name)
{
#ifdef _MSC_VER
    static_assert(std::is_trivially_constructible_v<T>, "For non-trivial types, MEM_new must be used.");
#else
    static_assert(std::is_trivial_v<T>, "For non-trivial types, MEM_new must be used.");
#endif
    return static_cast<T*>(MEM_new_array_uninitialized_aligned(length, sizeof(T), alignof(T), allocation_name));
}

template <typename T,
          typename... Args>
inline T* MEM_new(const char* allocation_name,
                  Args&&... args)
{
    void* buffer = vektor::mem_guarded::internal::mem_mallocN_aligned_ex(
        sizeof(T), alignof(T), allocation_name,
        std::is_trivially_destructible_v<T> ? vektor::mem_guarded::internal::DestructorType::Trivial
                                            : vektor::mem_guarded::internal::DestructorType::NonTrivial);
    return new (buffer) T(std::forward<Args>(args)...);
}

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

#define MEM_SAFE_DELETE(v)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        if (v)                                                                                                         \
        {                                                                                                              \
            MEM_delete(v);                                                                                             \
            (v) = nullptr;                                                                                             \
        }                                                                                                              \
    } while (0)
}

void* MEM_new_uninitialized(size_t      len,
                            const char* str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1)
    ATTR_NONNULL(2);

template <typename T>
inline T* MEM_new_uninitialized(const char* allocation_name)
{
    return static_cast<T*>(MEM_new_array_uninitialized_aligned(1, sizeof(T), alignof(T), allocation_name));
}