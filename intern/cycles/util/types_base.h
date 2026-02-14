

#include <cstddef>
#include <cstdint>

#include "defines.h"

using device_ptr = uint64_t;

ccl_device_inline size_t align_up(const size_t offset,
                                  const size_t alignment)
{
    return (offset + alignment - 1) & ~(alignment - 1);
}

ccl_device_inline size_t divide_up(const size_t x,
                                   const size_t y)
{
    return (x + y - 1) / y;
}

ccl_device_inline size_t round_up(const size_t x,
                                  const size_t multiple)
{
    return ((x + multiple - 1) / multiple) * multiple;
}

ccl_device_inline size_t round_down(const size_t x,
                                    const size_t multiple)
{
    return (x / multiple) * multiple;
}

ccl_device_inline bool is_power_of_two(const size_t x)
{
    return (x & (x - 1)) == 0;
}