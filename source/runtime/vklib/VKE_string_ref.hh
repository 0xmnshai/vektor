#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "VKE_span.hh"

namespace vektor {
namespace vklib {

class StringRef;

class StringRefBase {
protected:
  const char *data_;
  int64_t size_;

  constexpr StringRefBase(const char *data, int64_t size);

public:
  static constexpr int64_t not_found = -1;

  constexpr int64_t size() const;
  constexpr bool is_empty() const;
  constexpr const char *data() const;
  constexpr operator Span<char>() const;

  operator std::string() const;
  constexpr operator std::string_view() const;

  constexpr const char *begin() const;
  constexpr const char *end() const;

  constexpr IndexRange index_range() const;

  void copy_utf8_truncated(char *dst, int64_t dst_size) const;
  template <size_t N> void copy_utf8_truncated(char (&dst)[N]) const;

  void copy_bytes_truncated(char *dst, int64_t dst_size) const;
  template <size_t N> void copy_bytes_truncated(char (&dst)[N]) const;
  void copy_unsafe(char *dst) const;

  constexpr bool startswith(StringRef prefix) const;
  constexpr bool endswith(StringRef suffix) const;
  constexpr StringRef substr(int64_t start, int64_t size) const;

  constexpr const char &front() const;
  constexpr const char &back() const;

  constexpr int64_t find(char c, int64_t pos = 0) const;
  constexpr int64_t find(StringRef str, int64_t pos = 0) const;
  constexpr int64_t rfind(char c, int64_t pos = INT64_MAX) const;
  constexpr int64_t rfind(StringRef str, int64_t pos = INT64_MAX) const;
  constexpr int64_t find_first_of(StringRef chars, int64_t pos = 0) const;
  constexpr int64_t find_first_of(char c, int64_t pos = 0) const;
  constexpr int64_t find_last_of(StringRef chars,
                                 int64_t pos = INT64_MAX) const;
  constexpr int64_t find_last_of(char c, int64_t pos = INT64_MAX) const;
  constexpr int64_t find_first_not_of(StringRef chars, int64_t pos = 0) const;
  constexpr int64_t find_first_not_of(char c, int64_t pos = 0) const;
  constexpr int64_t find_last_not_of(StringRef chars,
                                     int64_t pos = INT64_MAX) const;
  constexpr int64_t find_last_not_of(char c, int64_t pos = INT64_MAX) const;

  constexpr StringRef trim() const;
  constexpr StringRef trim(StringRef characters_to_remove) const;
  constexpr StringRef trim(char character_to_remove) const;
};

class StringRefNull : public StringRefBase {

public:
  constexpr StringRefNull();
  constexpr StringRefNull(const char *str, int64_t size);
  StringRefNull(std::nullptr_t) = delete;
  constexpr StringRefNull(const char *str);
  StringRefNull(const std::string &str);

  constexpr char operator[](int64_t index) const;
  constexpr const char *c_str() const;
};

class StringRef : public StringRefBase {
public:
  constexpr StringRef();
  constexpr StringRef(StringRefNull other);
  constexpr StringRef(const char *str);
  constexpr StringRef(const char *str, int64_t length);
  constexpr StringRef(const char *begin, const char *one_after_end);
  constexpr StringRef(std::string_view view);
  constexpr StringRef(Span<char> span);
  StringRef(const std::string &str);

  constexpr StringRef drop_prefix(int64_t n) const;
  constexpr StringRef drop_known_prefix(StringRef prefix) const;
  constexpr StringRef drop_suffix(int64_t n) const;
  constexpr StringRef drop_known_suffix(StringRef suffix) const;

  constexpr char operator[](int64_t index) const;
};

constexpr StringRefBase::StringRefBase(const char *data, const int64_t size)
    : data_(data), size_(size) {}

constexpr int64_t StringRefBase::size() const { return size_; }

constexpr bool StringRefBase::is_empty() const { return size_ == 0; }

constexpr const char *StringRefBase::data() const { return data_; }

constexpr StringRefBase::operator Span<char>() const {
  return Span<char>(data_, size_);
}

inline StringRefBase::operator std::string() const {
  return std::string(data_, size_t(size_));
}

constexpr StringRefBase::operator std::string_view() const {
  return std::string_view(data_, size_t(size_));
}

constexpr const char *StringRefBase::begin() const { return data_; }

constexpr const char *StringRefBase::end() const { return data_ + size_; }

constexpr IndexRange StringRefBase::index_range() const {
  return IndexRange(size_);
}

inline void StringRefBase::copy_unsafe(char *dst) const {
  if (size_ > 0) {
    memcpy(dst, data_, size_t(size_));
  }
  dst[size_] = '\0';
}

template <size_t N>
inline void StringRefBase::copy_utf8_truncated(char (&dst)[N]) const {
  this->copy_utf8_truncated(dst, N);
}

template <size_t N>
inline void StringRefBase::copy_bytes_truncated(char (&dst)[N]) const {
  this->copy_bytes_truncated(dst, N);
}

inline void StringRefBase::copy_utf8_truncated(char *dst,
                                               int64_t dst_size) const {
  assert(dst_size > 0);
  int64_t copy_size = std::min(size_, dst_size - 1);
  assert(copy_size >= 0);

  std::memcpy(dst, data_, copy_size);



  if (copy_size < size_ && (data_[copy_size] & 0xC0) == 0x80) {
    while (copy_size > 0 && (dst[copy_size - 1] & 0xC0) == 0x80) {
      copy_size--;
    }
    if (copy_size > 0) {
      copy_size--;
    }
  }

  dst[copy_size] = '\0';
}

inline void StringRefBase::copy_bytes_truncated(char *dst,
                                                int64_t dst_size) const {
  assert(dst_size > 0);
  int64_t copy_size = std::min(size_, dst_size - 1);
  std::memcpy(dst, data_, copy_size);
  dst[copy_size] = '\0';
}

constexpr bool StringRefBase::startswith(StringRef prefix) const {
  if (size_ < prefix.size_) {
    return false;
  }
  for (int64_t i = 0; i < prefix.size_; i++) {
    if (data_[i] != prefix.data_[i]) {
      return false;
    }
  }
  return true;
}

constexpr bool StringRefBase::endswith(StringRef suffix) const {
  if (size_ < suffix.size_) {
    return false;
  }
  const int64_t offset = size_ - suffix.size_;
  for (int64_t i = 0; i < suffix.size_; i++) {
    if (data_[offset + i] != suffix.data_[i]) {
      return false;
    }
  }
  return true;
}

constexpr StringRef StringRefBase::substr(const int64_t start,
                                          const int64_t max_size) const {
  assert(max_size >= 0);
  assert(start >= 0);
  const int64_t substr_size = std::min(max_size, size_ - start);
  return StringRef(data_ + start, substr_size);
}

constexpr const char &StringRefBase::front() const {
  assert(size_ >= 1);
  return data_[0];
}

constexpr const char &StringRefBase::back() const {
  assert(size_ >= 1);
  return data_[size_ - 1];
}

constexpr int64_t index_or_npos_to_int64(size_t index) {
  if (index == std::string_view::npos) {
    return StringRef::not_found;
  }
  return int64_t(index);
}

constexpr int64_t StringRefBase::find(char c, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(std::string_view(*this).find(c, size_t(pos)));
}

constexpr int64_t StringRefBase::find(StringRef str, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(std::string_view(*this).find(str, size_t(pos)));
}

constexpr int64_t StringRefBase::rfind(char c, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(std::string_view(*this).rfind(c, size_t(pos)));
}

constexpr int64_t StringRefBase::rfind(StringRef str, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).rfind(str, size_t(pos)));
}

constexpr int64_t StringRefBase::find_first_of(StringRef chars,
                                               int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_first_of(chars, size_t(pos)));
}

constexpr int64_t StringRefBase::find_first_of(char c, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_first_of(c, size_t(pos)));
}

constexpr int64_t StringRefBase::find_last_of(StringRef chars,
                                              int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_last_of(chars, size_t(pos)));
}

constexpr int64_t StringRefBase::find_last_of(char c, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_last_of(c, size_t(pos)));
}

constexpr int64_t StringRefBase::find_first_not_of(StringRef chars,
                                                   int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_first_not_of(chars, size_t(pos)));
}

constexpr int64_t StringRefBase::find_first_not_of(char c, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_first_not_of(c, size_t(pos)));
}

constexpr int64_t StringRefBase::find_last_not_of(StringRef chars,
                                                  int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_last_not_of(chars, size_t(pos)));
}

constexpr int64_t StringRefBase::find_last_not_of(char c, int64_t pos) const {
  assert(pos >= 0);
  return index_or_npos_to_int64(
      std::string_view(*this).find_last_not_of(c, size_t(pos)));
}

constexpr StringRef StringRefBase::trim() const {
  return this->trim(" \t\r\n");
}

constexpr StringRef StringRefBase::trim(const char character_to_remove) const {
  return this->trim(StringRef(&character_to_remove, 1));
}

constexpr StringRef StringRefBase::trim(StringRef characters_to_remove) const {
  const int64_t find_front = this->find_first_not_of(characters_to_remove);
  if (find_front == not_found) {
    return StringRef();
  }
  const int64_t find_end = this->find_last_not_of(characters_to_remove);
  const int64_t substr_len = find_end - find_front + 1;
  return this->substr(find_front, substr_len);
}

constexpr StringRefNull::StringRefNull() : StringRefBase("", 0) {}

constexpr StringRefNull::StringRefNull(const char *str, const int64_t size)
    : StringRefBase(str, size) {
  assert(int64_t(std::char_traits<char>::length(str)) == size);
}

constexpr StringRefNull::StringRefNull(const char *str)
    : StringRefBase(str, int64_t(std::char_traits<char>::length(str))) {
  assert(str != nullptr);
}

inline StringRefNull::StringRefNull(const std::string &str)
    : StringRefNull(str.c_str(), int64_t(str.size())) {}

constexpr char StringRefNull::operator[](const int64_t index) const {
  assert(index >= 0);
  assert(index <= size_);
  return data_[index];
}

constexpr const char *StringRefNull::c_str() const { return data_; }

constexpr StringRef::StringRef() : StringRefBase(nullptr, 0) {}

constexpr StringRef::StringRef(StringRefNull other)
    : StringRefBase(other.data(), other.size()) {}

constexpr StringRef::StringRef(const char *str)
    : StringRefBase(str,
                    str ? int64_t(std::char_traits<char>::length(str)) : 0) {}

constexpr StringRef::StringRef(const char *str, const int64_t length)
    : StringRefBase(str, length) {}

constexpr StringRef StringRef::drop_prefix(const int64_t n) const {
  assert(n >= 0);
  const int64_t clamped_n = std::min(n, size_);
  const int64_t new_size = size_ - clamped_n;
  return StringRef(data_ + clamped_n, new_size);
}

constexpr StringRef StringRef::drop_known_prefix(StringRef prefix) const {
  assert(this->startswith(prefix));
  return this->drop_prefix(prefix.size());
}

constexpr StringRef StringRef::drop_suffix(const int64_t n) const {
  assert(n >= 0);
  const int64_t new_size = std::max<int64_t>(0, size_ - n);
  return StringRef(data_, new_size);
}

constexpr StringRef StringRef::drop_known_suffix(StringRef suffix) const {
  assert(this->endswith(suffix));
  return this->drop_suffix(suffix.size());
}

constexpr char StringRef::operator[](int64_t index) const {
  assert(index >= 0);
  assert(index < size_);
  return data_[index];
}

constexpr StringRef::StringRef(const char *begin, const char *one_after_end)
    : StringRefBase(begin, int64_t(one_after_end - begin)) {
  assert(begin <= one_after_end);
}

inline StringRef::StringRef(const std::string &str)
    : StringRefBase(str.data(), int64_t(str.size())) {}

constexpr StringRef::StringRef(std::string_view view)
    : StringRefBase(view.data(), int64_t(view.size())) {}

constexpr StringRef::StringRef(Span<char> span)
    : StringRefBase(span.data(), span.size()) {}

inline std::ostream &operator<<(std::ostream &stream, StringRef ref) {
  if (ref.size() > 0) {
    stream.write(ref.data(), ref.size());
  }
  return stream;
}

inline std::ostream &operator<<(std::ostream &stream, StringRefNull ref) {
  return stream << StringRef(ref);
}

inline std::string operator+(StringRef a, StringRef b) {
  return std::string(a) + std::string(b);
}

constexpr bool operator==(StringRef a, StringRef b) {
  return std::string_view(a) == std::string_view(b);
}

constexpr bool operator!=(StringRef a, StringRef b) {
  return std::string_view(a) != std::string_view(b);
}

constexpr bool operator<(StringRef a, StringRef b) {
  return std::string_view(a) < std::string_view(b);
}

constexpr bool operator>(StringRef a, StringRef b) {
  return std::string_view(a) > std::string_view(b);
}

constexpr bool operator<=(StringRef a, StringRef b) {
  return std::string_view(a) <= std::string_view(b);
}

constexpr bool operator>=(StringRef a, StringRef b) {
  return std::string_view(a) >= std::string_view(b);
}

}
}