#pragma once

#include <cstddef>

namespace mem_guarded::internal {

enum class DestructorType {
  /** Allocation has a trivial destructor. */
  Trivial,
  /** Allocation has a non-trivial destructor that should be called when freeing. */
  NonTrivial,
};

extern void (*mem_freeN_ex)(void *vmemh, DestructorType destructor_type);
extern void *(*mem_mallocN_aligned_ex)(size_t len,
                                       size_t alignment,
                                       const char *str,
                                       DestructorType destructor_type);
}  // namespace mem_guarded::internal
