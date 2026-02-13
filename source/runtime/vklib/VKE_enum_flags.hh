

#pragma once

#ifdef __cplusplus
#include <cstdint>

namespace vektor
{

template <typename T>
struct BitwiseNotEnumValue
{
    uint64_t value;
             operator uint64_t() const { return value; }
};

} // namespace vektor

#define ENUM_OPERATORS(_enum_type)                                                                                     \
    [[maybe_unused]] [[nodiscard]] inline constexpr _enum_type operator|(_enum_type a, _enum_type b)                   \
    {                                                                                                                  \
        return (_enum_type)(uint64_t(a) | uint64_t(b));                                                                \
    }                                                                                                                  \
    [[maybe_unused]] [[nodiscard]] inline constexpr _enum_type operator&(_enum_type a, _enum_type b)                   \
    {                                                                                                                  \
        return (_enum_type)(uint64_t(a) & uint64_t(b));                                                                \
    }                                                                                                                  \
    [[maybe_unused]] [[nodiscard]] inline constexpr _enum_type operator&(_enum_type                                a,  \
                                                                         ::vektor::BitwiseNotEnumValue<_enum_type> b)  \
    {                                                                                                                  \
        return (_enum_type)(uint64_t(a) & uint64_t(b.value));                                                          \
    }                                                                                                                  \
    [[maybe_unused]] [[nodiscard]] inline constexpr ::vektor::BitwiseNotEnumValue<_enum_type> operator~(_enum_type a)  \
    {                                                                                                                  \
        ::vektor::BitwiseNotEnumValue<_enum_type> result = {~uint64_t(a)};                                             \
        return result;                                                                                                 \
    }                                                                                                                  \
    [[maybe_unused]] inline _enum_type& operator|=(_enum_type& a, _enum_type b)                                        \
    {                                                                                                                  \
        return a = (_enum_type)(uint64_t(a) | uint64_t(b));                                                            \
    }                                                                                                                  \
    [[maybe_unused]] inline _enum_type& operator&=(_enum_type& a, _enum_type b)                                        \
    {                                                                                                                  \
        return a = (_enum_type)(uint64_t(a) & uint64_t(b));                                                            \
    }                                                                                                                  \
    [[maybe_unused]] inline _enum_type& operator&=(_enum_type& a, ::vektor::BitwiseNotEnumValue<_enum_type> b)         \
    {                                                                                                                  \
        return a = (_enum_type)(uint64_t(a) & uint64_t(b.value));                                                      \
    }                                                                                                                  \
    [[maybe_unused]] inline _enum_type& operator^=(_enum_type& a, _enum_type b)                                        \
    {                                                                                                                  \
        return a = (_enum_type)(uint64_t(a) ^ uint64_t(b));                                                            \
    }                                                                                                                  \
    [[maybe_unused]] [[nodiscard]] inline constexpr bool flag_is_set(_enum_type flags, _enum_type flag_to_test)        \
    {                                                                                                                  \
        return (uint64_t(flags) & uint64_t(flag_to_test)) != 0;                                                        \
    }

#else

#define ENUM_OPERATORS(_enum_type)

#endif
