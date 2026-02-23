#pragma once

#include <cstddef>

#include "mallocn_defines.hh"

namespace vektor
{
namespace mem_guarded
{
namespace internal
{

enum class DestructorType
{
    Trivial,
    NonTrivial,
};

extern void (*mem_freeN_ex)(void*          vmemh,
                            DestructorType destructor_type);

extern void* (*mem_callocN)(size_t      len,
                            const char* str)ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(2);

extern void* (*mem_calloc_arrayN)(size_t      len,
                                  size_t      size,
                                  const char* str)ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                          2) ATTR_NONNULL(3);

extern void* (*mem_mallocN)(size_t      len,
                            const char* str)ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(2);

extern void* (*mem_malloc_arrayN)(size_t      len,
                                  size_t      size,
                                  const char* str)ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                          2) ATTR_NONNULL(3);

extern void* (*mem_mallocN_aligned_ex)(size_t         len,
                                       size_t         alignment,
                                       const char*    str,
                                       DestructorType destructor_type);

extern void* (*mem_dupallocN)(const void* vmemh)ATTR_WARN_UNUSED_RESULT;

} // namespace internal
} // namespace mem_guarded
} // namespace vektor