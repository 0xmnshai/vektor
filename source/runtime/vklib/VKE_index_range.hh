#pragma once

#include <cassert>
#include <cstdint>
#include <iterator>

namespace vektor
{
namespace vklib
{

class IndexRange
{
private:
    int64_t start_;
    int64_t size_;

public:
    class Iterator
    {
    private:
        int64_t current_;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = int64_t;
        using difference_type   = int64_t;
        using pointer           = const int64_t*;
        using reference         = const int64_t&;

        constexpr Iterator(int64_t current)
            : current_(current)
        {
        }

        constexpr int64_t operator*() const { return current_; }

        const Iterator&   operator++()
        {
            current_++;
            return *this;
        }

        const Iterator operator++(int)
        {
            Iterator copy = *this;
            current_++;
            return copy;
        }

        const Iterator& operator--()
        {
            current_--;
            return *this;
        }

        const Iterator operator--(int)
        {
            Iterator copy = *this;
            current_--;
            return copy;
        }

        constexpr bool     operator==(const Iterator& other) const { return current_ == other.current_; }

        constexpr bool     operator!=(const Iterator& other) const { return current_ != other.current_; }

        constexpr Iterator operator+(int64_t n) const { return Iterator(current_ + n); }

        constexpr Iterator operator-(int64_t n) const { return Iterator(current_ - n); }

        constexpr int64_t  operator-(const Iterator& other) const { return current_ - other.current_; }
    };

    constexpr IndexRange()
        : start_(0)
        , size_(0)
    {
    }

    constexpr IndexRange(int64_t size)
        : start_(0)
        , size_(size)
    {
        assert(size >= 0);
    }

    constexpr IndexRange(int64_t start,
                         int64_t size)
        : start_(start)
        , size_(size)
    {
        assert(size >= 0);
    }

    constexpr int64_t  start() const { return start_; }

    constexpr int64_t  size() const { return size_; }

    constexpr int64_t  end_val() const { return start_ + size_; }

    constexpr bool     is_empty() const { return size_ == 0; }

    constexpr Iterator begin() const { return Iterator(start_); }

    constexpr Iterator end() const { return Iterator(start_ + size_); }

    constexpr int64_t  operator[](int64_t index) const
    {
        assert(index >= 0);
        assert(index < size_);
        return start_ + index;
    }
};
} // namespace vklib
} // namespace vektor
