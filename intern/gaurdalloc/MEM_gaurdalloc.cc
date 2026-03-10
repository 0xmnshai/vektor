#include "MEM_gaurdalloc.hh"
#include "MEM_function_pointers.h"
#include <cstdlib>
#include <cstring>

namespace mem_guarded::internal {

static size_t total_memory_in_use = 0;

void (*mem_freeN_ex)(void *vmemh, DestructorType /*destructor_type*/) =
    [](void *vmemh, DestructorType /*destructor_type*/) {
      if (vmemh) {
        std::free(vmemh);
      }
    };

void *(*mem_mallocN_aligned_ex)(size_t len,
                                size_t alignment,
                                const char * /*str*/,
                                DestructorType /*destructor_type*/) =
    [](size_t len,
       size_t alignment,
       const char * /*str*/,
       DestructorType /*destructor_type*/) -> void * {
  void *ptr = nullptr;
  if (alignment <= alignof(std::max_align_t)) {
    ptr = std::malloc(len);
  }
  else {
    if (posix_memalign(&ptr, alignment, len) != 0) {
      return nullptr;
    }
  }
  if (ptr) {
    total_memory_in_use += len;
  }
  return ptr;
};
}  // namespace mem_guarded::internal

namespace mem {

void *(*MEM_new_array_zeroed_aligned)(size_t len, size_t size, size_t alignment, const char *str) =
    [](size_t len, size_t size, size_t alignment, const char *str) -> void * {
  size_t total_size = len * size;
  void *ptr = mem_guarded::internal::mem_mallocN_aligned_ex(
      total_size, alignment, str, mem_guarded::internal::DestructorType::Trivial);
  if (ptr) {
    std::memset(ptr, 0, total_size);
  }
  return ptr;
};

void *MEM_new_array_uninitialized(size_t len, size_t size, const char *str)
{
  return mem_guarded::internal::mem_mallocN_aligned_ex(
      len * size, alignof(std::max_align_t), str, mem_guarded::internal::DestructorType::Trivial);
}

void *(*MEM_realloc_uninitialized_id)(void *vmemh, size_t len, const char * /*str*/) =
    [](void *vmemh, size_t len, const char * /*str*/) -> void * {
  return std::realloc(vmemh, len);
};

size_t (*MEM_get_memory_in_use)() = []() -> size_t {
  return mem_guarded::internal::total_memory_in_use;
};

}  // namespace mem
