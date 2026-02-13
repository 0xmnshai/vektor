
#include <stddef.h>
#include <cstdint>

#include "mallocn_intern.hh"

#define MEM_INLINE static inline

MEM_INLINE bool MEM_size_safe_multiply(size_t  a,
                                       size_t  b,
                                       size_t* result)
{
    const size_t high_bits = SIZE_MAX << (sizeof(size_t) * 8 / 2);
    *result                = a * b;

    if (UNLIKELY(*result == 0))
    {
        return (a == 0 || b == 0);
    }

    return ((high_bits & (a | b)) == 0 || (*result / b == a));
}