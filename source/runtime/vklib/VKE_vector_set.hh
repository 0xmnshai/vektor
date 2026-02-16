#pragma once

#include <cstdint>
#include "VKE_memory_util.hh"

namespace vektor
{

template <typename Key, int64_t InlineBufferCapacity = default_inline_buffer_capacity(sizeof(Key))>
class VectorSet
{
public:
    using value_type      = Key;
    using pointer         = Key*;
    using const_pointer   = const Key*;
    using reference       = Key&;
    using const_reference = const Key&;
    using iterator        = Key*;
    using const_iterator  = const Key*;
    using size_type       = int64_t;
};
} // namespace vektor