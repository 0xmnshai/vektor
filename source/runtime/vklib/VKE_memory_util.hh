#pragma once

#include <cstdint>
#include <type_traits>

namespace vektor
{

template <typename T, typename... Args>
inline constexpr bool is_same_any_v = (std::is_same<T, Args>::value || ...);

constexpr int64_t     default_inline_buffer_capacity(size_t element_size)
{
    if (is_same_any_v<decltype(element_size), int64_t, size_t>)
    {
        return (int64_t(element_size) < 100) ? 4 : 0;
    }
    else
    {
        return 0;
    }
}

} // namespace vektor