#pragma once

#include <array>
#include <cassert>
#include <type_traits>
#include <vector>

#include "VKE_index_range.hh"

namespace vektor
{
namespace vklib
{

template <typename T>
class Span;
template <typename T>
class MutableSpan;

namespace span_internal
{
template <typename From, typename To>
constexpr bool is_span_convertible_pointer_v = std::is_convertible<From (*)[], To (*)[]>::value;
}

template <typename T>
class Span
{
public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using iterator        = const T*;
    using size_type       = int64_t;

protected:
    const T* data_ = nullptr;
    int64_t  size_ = 0;

public:
    Span() = default;

    Span(const T* start,
         int64_t  size)
        : data_(start)
        , size_(size)
    {
        assert(size >= 0);
    }

    template <typename U,
              std::enable_if<span_internal::is_span_convertible_pointer_v<U,
                                                                          T>,
                             int> = 0>
    Span(const U* start,
         int64_t  size)
        : data_(static_cast<const T*>(start))
        , size_(size)
    {
        assert(size >= 0);
    }

    Span(const std::initializer_list<T>& list)
        : Span(list.begin(),
               int64_t(list.size()))
    {
    }

    Span(const std::vector<T>& vector)
        : Span(vector.data(),
               int64_t(vector.size()))
    {
    }

    template <std::size_t N>
    Span(const std::array<T,
                          N>& array)
        : Span(array.data(),
               N)
    {
    }

    template <typename U,
              std::enable_if<span_internal::is_span_convertible_pointer_v<U,
                                                                          T>,
                             int> = 0>
    Span(Span<U> span)
        : data_(static_cast<const T*>(span.data()))
        , size_(span.size())
    {
    }

    Span slice(int64_t start,
               int64_t size) const
    {
        assert(start >= 0);
        assert(size >= 0);
        if (size == 0)
        {
            return Span(data_ ? data_ + start : nullptr, 0);
        }
        assert(start + size <= size_);
        return Span(data_ + start, size);
    }

    Span slice(IndexRange range) const { return this->slice(range.start(), range.size()); }

    Span slice_safe(const int64_t start,
                    const int64_t size) const
    {
        assert(start >= 0);
        assert(size >= 0);
        const int64_t new_size = std::max<int64_t>(0, std::min(size, size_ - start));
        return Span(data_ ? data_ + start : nullptr, new_size);
    }

    Span slice_safe(IndexRange range) const { return this->slice_safe(range.start(), range.size()); }

    Span drop_front(int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::max<int64_t>(0, size_ - n);
        return Span(data_ + n, new_size);
    }

    Span drop_back(int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::max<int64_t>(0, size_ - n);
        return Span(data_, new_size);
    }

    Span take_front(int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::min<int64_t>(size_, n);
        return Span(data_, new_size);
    }

    Span take_back(int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::min<int64_t>(size_, n);
        return Span(data_ + size_ - new_size, new_size);
    }

    const T*                        data() const { return data_; }

    const T*                        begin() const { return data_; }
    const T*                        end() const { return data_ + size_; }

    std::reverse_iterator<const T*> rbegin() const { return std::reverse_iterator<const T*>(this->end()); }
    std::reverse_iterator<const T*> rend() const { return std::reverse_iterator<const T*>(this->begin()); }

    const T&                        operator[](int64_t index) const
    {
        assert(index >= 0);
        assert(index < size_);
        return data_[index];
    }

    int64_t size() const { return size_; }

    bool    is_empty() const { return size_ == 0; }

    int64_t size_in_bytes() const { return sizeof(T) * size_; }

    bool    contains(const T& value) const
    {
        for (const T& element : *this)
        {
            if (element == value)
            {
                return true;
            }
        }
        return false;
    }

    bool    contains_ptr(const T* ptr) const { return (this->begin() <= ptr) && (ptr < this->end()); }

    int64_t count(const T& value) const
    {
        int64_t counter = 0;
        for (const T& element : *this)
        {
            if (element == value)
            {
                counter++;
            }
        }
        return counter;
    }

    const T& first() const
    {
        assert(size_ > 0);
        return data_[0];
    }

    const T& last(const int64_t n = 0) const
    {
        assert(n >= 0);
        assert(n < size_);
        return data_[size_ - 1 - n];
    }

    int64_t first_index(const T& search_value) const
    {
        const int64_t index = this->first_index_try(search_value);
        assert(index >= 0);
        return index;
    }

    int64_t first_index_try(const T& search_value) const
    {
        for (int64_t i = 0; i < size_; i++)
        {
            if (data_[i] == search_value)
            {
                return i;
            }
        }
        return -1;
    }

    IndexRange index_range() const { return IndexRange(size_); }
    template <typename NewT>
    Span<NewT> cast() const
    {
        assert((size_ * sizeof(T)) % sizeof(NewT) == 0);
        int64_t new_size = size_ * sizeof(T) / sizeof(NewT);
        return Span<NewT>(reinterpret_cast<const NewT*>(data_), new_size);
    }

    friend bool operator==(const Span<T> a,
                           const Span<T> b)
    {
        if (a.size() != b.size())
        {
            return false;
        }
        return std::equal(a.begin(), a.end(), b.begin());
    }

    friend bool operator!=(const Span<T> a,
                           const Span<T> b)
    {
        return !(a == b);
    }
};

template <typename T>
class MutableSpan
{
public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using iterator        = T*;
    using size_type       = int64_t;

protected:
    T*      data_ = nullptr;
    int64_t size_ = 0;

public:
    MutableSpan() = default;

    MutableSpan(T*            start,
                const int64_t size)
        : data_(start)
        , size_(size)
    {
    }

    MutableSpan(std::vector<T>& vector)
        : MutableSpan(vector.data(),
                      vector.size())
    {
    }

    template <std::size_t N>
    MutableSpan(std::array<T,
                           N>& array)
        : MutableSpan(array.data(),
                      N)
    {
    }

    template <typename U,
              std::enable_if<span_internal::is_span_convertible_pointer_v<U,
                                                                          T>,
                             int> = 0>
    MutableSpan(MutableSpan<U> span)
        : data_(static_cast<T*>(span.data()))
        , size_(span.size())
    {
    }

    operator Span<T>() const { return Span<T>(data_, size_); }

    template <typename U,
              std::enable_if<span_internal::is_span_convertible_pointer_v<T,
                                                                          U>,
                             int> = 0>
    operator Span<U>() const
    {
        return Span<U>(static_cast<const U*>(data_), size_);
    }

    int64_t size() const { return size_; }

    int64_t size_in_bytes() const { return sizeof(T) * size_; }

    bool    is_empty() const { return size_ == 0; }

    void    fill(const T& value) const { std::fill_n(data_, size_, value); }
    template <typename IndexT>
    void fill_indices(Span<IndexT> indices,
                      const T&     value) const
    {
        for (IndexT i : indices)
        {
            assert(i < size_);
            data_[i] = value;
        }
    }

    T*                        data() const { return data_; }

    T*                        begin() const { return data_; }
    T*                        end() const { return data_ + size_; }

    std::reverse_iterator<T*> rbegin() const { return std::reverse_iterator<T*>(this->end()); }
    std::reverse_iterator<T*> rend() const { return std::reverse_iterator<T*>(this->begin()); }

    T&                        operator[](const int64_t index) const
    {
        assert(index >= 0);
        assert(index < size_);
        return data_[index];
    }

    MutableSpan slice(const int64_t start,
                      const int64_t size) const
    {
        assert(start >= 0);
        assert(size >= 0);
        if (size == 0)
        {
            return MutableSpan(data_ ? data_ + start : nullptr, 0);
        }
        assert(start + size <= size_);
        return MutableSpan(data_ + start, size);
    }

    MutableSpan slice(IndexRange range) const { return this->slice(range.start(), range.size()); }
    MutableSpan slice_safe(const int64_t start,
                           const int64_t size) const
    {
        assert(start >= 0);
        assert(size >= 0);
        const int64_t new_size = std::max<int64_t>(0, std::min(size, size_ - start));
        return MutableSpan(data_ + start, new_size);
    }

    MutableSpan slice_safe(IndexRange range) const { return this->slice_safe(range.start(), range.size()); }

    MutableSpan drop_front(const int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::max<int64_t>(0, size_ - n);
        return MutableSpan(data_ + n, new_size);
    }

    MutableSpan drop_back(const int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::max<int64_t>(0, size_ - n);
        return MutableSpan(data_, new_size);
    }

    MutableSpan take_front(const int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::min<int64_t>(size_, n);
        return MutableSpan(data_, new_size);
    }

    MutableSpan take_back(const int64_t n) const
    {
        assert(n >= 0);
        const int64_t new_size = std::min<int64_t>(size_, n);
        return MutableSpan(data_ + size_ - new_size, new_size);
    }

    void       reverse() const { std::reverse(data_, data_ + size_); }

    Span<T>    as_span() const { return Span<T>(data_, size_); }

    IndexRange index_range() const { return IndexRange(size_); }

    T&         first() const
    {
        assert(size_ > 0);
        return data_[0];
    }

    T& last(const int64_t n = 0) const
    {
        assert(n >= 0);
        assert(n < size_);
        return data_[size_ - 1 - n];
    }

    int64_t count(const T& value) const
    {
        int64_t counter = 0;
        for (const T& element : *this)
        {
            if (element == value)
            {
                counter++;
            }
        }
        return counter;
    }

    bool contains(const T& value) const
    {
        for (const T& element : *this)
        {
            if (element == value)
            {
                return true;
            }
        }
        return false;
    }

    bool contains_ptr(const T* ptr) const { return (this->begin() <= ptr) && (ptr < this->end()); }

    void copy_from(Span<T> values) const
    {
        assert(size_ == values.size());
        std::copy_n(values.data(), size_, data_);
    }

    template <typename NewT>
    MutableSpan<NewT> cast() const
    {
        assert((size_ * sizeof(T)) % sizeof(NewT) == 0);
        int64_t new_size = size_ * sizeof(T) / sizeof(NewT);
        return MutableSpan<NewT>(reinterpret_cast<NewT*>(data_), new_size);
    }
};
} // namespace vklib
} // namespace vektor