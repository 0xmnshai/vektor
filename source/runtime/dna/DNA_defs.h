#pragma once

#include <cstddef>
#ifdef DNA_DEPRECATED_ALLOW
#  define DNA_DEPRECATED
#else
#  if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
#    define DNA_DEPRECATED [[deprecated]]
#  else
#    define DNA_DEPRECATED
#  endif
#endif

#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#  define DNA_DEPRECATED_IGNORE(statement) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"") \
    statement \
    _Pragma("GCC diagnostic pop")
/* clang-format on */
#else
#  define DNA_DEPRECATED_IGNORE(statement) statement
#endif

namespace vektor::dna {
#define MAX_NAME 64
void DNA_internal_memcpy(void *dst, const void *src, size_t size);
void DNA_internal_memzero(void *dst, size_t size);
void DNA_internal_swap(void *a, void *b, size_t size);

template<class T> class ShallowDataConstRef {
    
};

}  // namespace vektor::dna
