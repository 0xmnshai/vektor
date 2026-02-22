#pragma once

namespace vektor
{
namespace vklib
{

template <typename... Args>
inline void UNUSED_VARS(Args&&... args)
{
    (void)(sizeof...(args));
}

#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
} // namespace vklib
}; // namespace vektor