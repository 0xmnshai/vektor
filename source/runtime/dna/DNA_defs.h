

#pragma once

#include <stddef.h>
#include <cstdint>
#ifdef DNA_DEPRECATED_ALLOW

#define DNA_DEPRECATED
#else
#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
#define DNA_DEPRECATED [[deprecated]]
#else
#define DNA_DEPRECATED
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)

#define DNA_DEPRECATED_IGNORE(statement)                                                                               \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")                     \
        statement _Pragma("GCC diagnostic pop")

#else
#define DNA_DEPRECATED_IGNORE(statement) statement
#endif

#ifdef __GNUC__
#define DNA_PRIVATE_ATTR __attribute__((deprecated))
#else
#define DNA_PRIVATE_ATTR
#endif

#ifdef DNA_DEPRECATED_ALLOW
#define DNA_DEPRECATED_GCC_POISON 0
#else

#ifdef __GNUC__
#define DNA_DEPRECATED_GCC_POISON 1
#else
#define DNA_DEPRECATED_GCC_POISON 0
#endif
#endif

namespace
{

#define MAX_NAME 64

void _DNA_internal_memcpy(void*       dst,
                          const void* src,
                          size_t      size);
void _DNA_internal_memzero(void*  dst,
                           size_t size);
void _DNA_internal_swap(void*  a,
                        void*  b,
                        size_t size);

namespace vektor
{

template <class T>
class ShallowDataConstRef
{
public:
    constexpr explicit ShallowDataConstRef(const T& ref)
        : ref_(ref)
    {
    }

    inline const T* get_pointer() const { return &ref_; }

private:
    const T& ref_;
};

class ShallowZeroInitializeTag
{
};

} // namespace vektor

#define DNA_DEFINE_CXX_METHODS(class_name)                                                                             \
    DNA_DEPRECATED_IGNORE(class_name() = default;)                                                                     \
    ~class_name()                                  = default;                                                          \
                                                                                                                       \
    class_name(const class_name& other)            = delete;                                                           \
    class_name(class_name&& other) noexcept        = delete;                                                           \
    class_name& operator=(const class_name& other) = delete;                                                           \
    class_name& operator=(class_name&& other)      = delete;                                                           \
                                                                                                                       \
    class_name(const dna::internal::ShallowDataConstRef<class_name> ref)                                               \
        : class_name()                                                                                                 \
    {                                                                                                                  \
        _DNA_internal_memcpy(this, ref.get_pointer(), sizeof(class_name));                                             \
    }                                                                                                                  \
    class_name& operator=(const dna::internal::ShallowDataConstRef<class_name> ref)                                    \
    {                                                                                                                  \
        if (this != ref.get_pointer())                                                                                 \
        {                                                                                                              \
            _DNA_internal_memcpy(this, ref.get_pointer(), sizeof(class_name));                                         \
        }                                                                                                              \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    class_name(const dna::internal::ShallowZeroInitializeTag)                                                          \
        : class_name()                                                                                                 \
    {                                                                                                                  \
        _DNA_internal_memzero(this, sizeof(class_name));                                                               \
    }                                                                                                                  \
    class_name& operator=(const dna::internal::ShallowZeroInitializeTag)                                               \
    {                                                                                                                  \
        _DNA_internal_memzero(this, sizeof(class_name));                                                               \
        return *this;                                                                                                  \
    }

template <typename T>
inline void shallow_copy_array(T*            dst,
                               const T*      src,
                               const int64_t size)
{
    _DNA_internal_memcpy(dst, src, sizeof(T) * size_t(size));
}

template <typename T>
inline void shallow_swap(T& a,
                         T& b)
{
    _DNA_internal_swap(&a, &b, sizeof(T));
}

} // namespace