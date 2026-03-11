#pragma once

#include <utility>

#include "../../source/runtime/lib/VLI_complier_attrs.h"
#include "MEM_function_pointers.h"

namespace mem {
template<typename T, typename... Args>
inline T *MEM_new(const char *allocation_name, Args &&...args)
{
  void *buffer = mem_guarded::internal::mem_mallocN_aligned_ex(
      sizeof(T),
      alignof(T),
      allocation_name,
      std::is_trivially_destructible_v<T> ? mem_guarded::internal::DestructorType::Trivial :
                                            mem_guarded::internal::DestructorType::NonTrivial);
  return new (buffer) T(std::forward<Args>(args)...);
}

extern void *(*MEM_new_array_zeroed_aligned)(
    size_t len,
    size_t size,
    size_t alignment,
    const char *str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1, 2)
    ATTR_NONNULL(4);

void *MEM_new_array_uninitialized(size_t len,
                                  size_t size,
                                  const char *str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT
    ATTR_ALLOC_SIZE(1, 2) ATTR_NONNULL(3);

template<typename T> inline void MEM_delete(const T *ptr)
{
  static_assert(
      !std::is_void_v<T>,
      "MEM_delete on a void pointer is not possible, `static_cast` it to the correct type");
  if (ptr == nullptr) {
    return;
  }
  const void *complete_ptr = [ptr]() {
    if constexpr (std::is_polymorphic_v<T>) {
      return dynamic_cast<const void *>(ptr);
    }
    else {
      return static_cast<const void *>(ptr);
    }
  }();
  if constexpr (!std::is_trivially_destructible_v<T>) {
    ptr->~T();
  }
  mem_guarded::internal::mem_freeN_ex(const_cast<void *>(complete_ptr),
                                      mem_guarded::internal::DestructorType::NonTrivial);
}

extern void *(*MEM_realloc_uninitialized_id)(
    void *vmemh, size_t len, const char *str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT
    ATTR_ALLOC_SIZE(2);

extern size_t (*MEM_get_memory_in_use)();

#define MEM_realloc_uninitialized(vmemh, len) MEM_realloc_uninitialized_id(vmemh, len, __func__)

#define MEM_SAFE_DELETE(v) \
  do { \
    if (v) { \
      MEM_delete(v); \
      (v) = nullptr; \
    } \
  } while (0)

template<typename T> inline T *MEM_new_zeroed(const char *allocation_name)
{
  static_assert(std::is_trivial_v<T>, "For non-trivial types, MEM_new must be used.");
  return static_cast<T *>(MEM_new_array_zeroed_aligned(1, sizeof(T), alignof(T), allocation_name));
}

#define MEM_mallocN(len, str) mem_guarded::internal::mem_mallocN(len, str)

#define MEM_freeN(vmemh) \
  mem_guarded::internal::mem_freeN_ex(vmemh, mem_guarded::internal::DestructorType::Trivial)

#define MEM_freeN_ex(vmemh, destructor_type) \
  mem_guarded::internal::mem_freeN_ex(vmemh, destructor_type)
}  // namespace mem
