#pragma once

#include <array>
#include <cassert>
#include <type_traits>
#include <vector>

#include "VKE_index_range.hh"

namespace vektor {
namespace vklib {

template <typename T> class Span;
template <typename T> class MutableSpan;

namespace span_internal {
template <typename From, typename To>
constexpr bool is_span_convertible_pointer_v =
    std::is_convertible<From (*)[], To (*)[]>::value;
}

template <typename T> class Span {
public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using iterator = const T *;
  using size_type = int64_t;

protected:
  const T *data_ = nullptr;
  int64_t size_ = 0;

public:
  constexpr Span() = default;

  constexpr Span(const T *start, int64_t size) : data_(start), size_(size) {
    assert(size >= 0);
  }

  template <typename U,
            std::enable_if_t<span_internal::is_span_convertible_pointer_v<U, T>,
                             int> = 0>
  constexpr Span(const U *start, int64_t size)
      : data_(static_cast<const T *>(start)), size_(size) {
    assert(size >= 0);
  }

  constexpr Span(const std::initializer_list<T> &list)
      : Span(list.begin(), int64_t(list.size())) {}

  constexpr Span(const std::vector<T> &vector)
      : Span(vector.data(), int64_t(vector.size())) {}

  template <std::size_t N>
  constexpr Span(const std::array<T, N> &array) : Span(array.data(), N) {}

  template <typename U,
            std::enable_if_t<span_internal::is_span_convertible_pointer_v<U, T>,
                             int> = 0>
  constexpr Span(Span<U> span)
      : data_(static_cast<const T *>(span.data())), size_(span.size()) {}

  constexpr Span slice(int64_t start, int64_t size) const {
    assert(start >= 0);
    assert(size >= 0);
    if (size == 0) {
      return Span(data_ ? data_ + start : nullptr, 0);
    }
    assert(start + size <= size_);
    return Span(data_ + start, size);
  }

  constexpr Span slice(IndexRange range) const {
    return this->slice(range.start(), range.size());
  }

  constexpr Span slice_safe(const int64_t start, const int64_t size) const {
    assert(start >= 0);
    assert(size >= 0);
    const int64_t new_size =
        std::max<int64_t>(0, std::min(size, size_ - start));
    return Span(data_ ? data_ + start : nullptr, new_size);
  }

  constexpr Span slice_safe(IndexRange range) const {
    return this->slice_safe(range.start(), range.size());
  }

  constexpr Span drop_front(int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::max<int64_t>(0, size_ - n);
    return Span(data_ + n, new_size);
  }

  constexpr Span drop_back(int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::max<int64_t>(0, size_ - n);
    return Span(data_, new_size);
  }

  constexpr Span take_front(int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::min<int64_t>(size_, n);
    return Span(data_, new_size);
  }

  constexpr Span take_back(int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::min<int64_t>(size_, n);
    return Span(data_ + size_ - new_size, new_size);
  }

  constexpr const T *data() const { return data_; }

  constexpr const T *begin() const { return data_; }
  constexpr const T *end() const { return data_ + size_; }

  constexpr std::reverse_iterator<const T *> rbegin() const {
    return std::reverse_iterator<const T *>(this->end());
  }
  constexpr std::reverse_iterator<const T *> rend() const {
    return std::reverse_iterator<const T *>(this->begin());
  }

  constexpr const T &operator[](int64_t index) const {
    assert(index >= 0);
    assert(index < size_);
    return data_[index];
  }

  constexpr int64_t size() const { return size_; }

  constexpr bool is_empty() const { return size_ == 0; }

  constexpr int64_t size_in_bytes() const { return sizeof(T) * size_; }

  constexpr bool contains(const T &value) const {
    for (const T &element : *this) {
      if (element == value) {
        return true;
      }
    }
    return false;
  }

  constexpr bool contains_ptr(const T *ptr) const {
    return (this->begin() <= ptr) && (ptr < this->end());
  }

  constexpr int64_t count(const T &value) const {
    int64_t counter = 0;
    for (const T &element : *this) {
      if (element == value) {
        counter++;
      }
    }
    return counter;
  }

  constexpr const T &first() const {
    assert(size_ > 0);
    return data_[0];
  }

  constexpr const T &last(const int64_t n = 0) const {
    assert(n >= 0);
    assert(n < size_);
    return data_[size_ - 1 - n];
  }

  constexpr int64_t first_index(const T &search_value) const {
    const int64_t index = this->first_index_try(search_value);
    assert(index >= 0);
    return index;
  }

  constexpr int64_t first_index_try(const T &search_value) const {
    for (int64_t i = 0; i < size_; i++) {
      if (data_[i] == search_value) {
        return i;
      }
    }
    return -1;
  }

  constexpr IndexRange index_range() const { return IndexRange(size_); }
  template <typename NewT> Span<NewT> constexpr cast() const {
    assert((size_ * sizeof(T)) % sizeof(NewT) == 0);
    int64_t new_size = size_ * sizeof(T) / sizeof(NewT);
    return Span<NewT>(reinterpret_cast<const NewT *>(data_), new_size);
  }

  friend bool operator==(const Span<T> a, const Span<T> b) {
    if (a.size() != b.size()) {
      return false;
    }
    return std::equal(a.begin(), a.end(), b.begin());
  }

  friend bool operator!=(const Span<T> a, const Span<T> b) { return !(a == b); }
};

template <typename T> class MutableSpan {
public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using iterator = T *;
  using size_type = int64_t;

protected:
  T *data_ = nullptr;
  int64_t size_ = 0;

public:
  constexpr MutableSpan() = default;

  constexpr MutableSpan(T *start, const int64_t size)
      : data_(start), size_(size) {}

  constexpr MutableSpan(std::vector<T> &vector)
      : MutableSpan(vector.data(), vector.size()) {}

  template <std::size_t N>
  constexpr MutableSpan(std::array<T, N> &array)
      : MutableSpan(array.data(), N) {}

  template <typename U,
            std::enable_if_t<span_internal::is_span_convertible_pointer_v<U, T>,
                             int> = 0>
  constexpr MutableSpan(MutableSpan<U> span)
      : data_(static_cast<T *>(span.data())), size_(span.size()) {}

  constexpr operator Span<T>() const { return Span<T>(data_, size_); }

  template <typename U,
            std::enable_if_t<span_internal::is_span_convertible_pointer_v<T, U>,
                             int> = 0>
  constexpr operator Span<U>() const {
    return Span<U>(static_cast<const U *>(data_), size_);
  }

  constexpr int64_t size() const { return size_; }

  constexpr int64_t size_in_bytes() const { return sizeof(T) * size_; }

  constexpr bool is_empty() const { return size_ == 0; }

  constexpr void fill(const T &value) const {
    std::fill_n(data_, size_, value);
  }
  template <typename IndexT>
  constexpr void fill_indices(Span<IndexT> indices, const T &value) const {
    for (IndexT i : indices) {
      assert(i < size_);
      data_[i] = value;
    }
  }

  constexpr T *data() const { return data_; }

  constexpr T *begin() const { return data_; }
  constexpr T *end() const { return data_ + size_; }

  constexpr std::reverse_iterator<T *> rbegin() const {
    return std::reverse_iterator<T *>(this->end());
  }
  constexpr std::reverse_iterator<T *> rend() const {
    return std::reverse_iterator<T *>(this->begin());
  }

  constexpr T &operator[](const int64_t index) const {
    assert(index >= 0);
    assert(index < size_);
    return data_[index];
  }

  constexpr MutableSpan slice(const int64_t start, const int64_t size) const {
    assert(start >= 0);
    assert(size >= 0);
    if (size == 0) {
      return MutableSpan(data_ ? data_ + start : nullptr, 0);
    }
    assert(start + size <= size_);
    return MutableSpan(data_ + start, size);
  }

  constexpr MutableSpan slice(IndexRange range) const {
    return this->slice(range.start(), range.size());
  }
  constexpr MutableSpan slice_safe(const int64_t start,
                                   const int64_t size) const {
    assert(start >= 0);
    assert(size >= 0);
    const int64_t new_size =
        std::max<int64_t>(0, std::min(size, size_ - start));
    return MutableSpan(data_ + start, new_size);
  }

  constexpr MutableSpan slice_safe(IndexRange range) const {
    return this->slice_safe(range.start(), range.size());
  }

  constexpr MutableSpan drop_front(const int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::max<int64_t>(0, size_ - n);
    return MutableSpan(data_ + n, new_size);
  }

  constexpr MutableSpan drop_back(const int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::max<int64_t>(0, size_ - n);
    return MutableSpan(data_, new_size);
  }

  constexpr MutableSpan take_front(const int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::min<int64_t>(size_, n);
    return MutableSpan(data_, new_size);
  }

  constexpr MutableSpan take_back(const int64_t n) const {
    assert(n >= 0);
    const int64_t new_size = std::min<int64_t>(size_, n);
    return MutableSpan(data_ + size_ - new_size, new_size);
  }

  constexpr void reverse() const { std::reverse(data_, data_ + size_); }

  constexpr Span<T> as_span() const { return Span<T>(data_, size_); }

  constexpr IndexRange index_range() const { return IndexRange(size_); }

  constexpr T &first() const {
    assert(size_ > 0);
    return data_[0];
  }

  constexpr T &last(const int64_t n = 0) const {
    assert(n >= 0);
    assert(n < size_);
    return data_[size_ - 1 - n];
  }

  constexpr int64_t count(const T &value) const {
    int64_t counter = 0;
    for (const T &element : *this) {
      if (element == value) {
        counter++;
      }
    }
    return counter;
  }

  constexpr bool contains(const T &value) const {
    for (const T &element : *this) {
      if (element == value) {
        return true;
      }
    }
    return false;
  }

  constexpr bool contains_ptr(const T *ptr) const {
    return (this->begin() <= ptr) && (ptr < this->end());
  }

  constexpr void copy_from(Span<T> values) const {
    assert(size_ == values.size());
    std::copy_n(values.data(), size_, data_);
  }

  template <typename NewT> constexpr MutableSpan<NewT> cast() const {
    assert((size_ * sizeof(T)) % sizeof(NewT) == 0);
    int64_t new_size = size_ * sizeof(T) / sizeof(NewT);
    return MutableSpan<NewT>(reinterpret_cast<NewT *>(data_), new_size);
  }
};
}
}