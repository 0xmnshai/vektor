
#pragma once

#include <cstddef>
#include <iterator>

#include "../vklib/VKE_compiler_compat.h"

namespace vektor
{
struct Link
{
    struct Link *next, *prev;
};

struct LinkData
{
    struct LinkData *next, *prev;
    void*            data;
};

struct ListBase
{
    void *first, *last;
};

#ifdef __cplusplus

template <typename T>
struct ListBaseTIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

private:
    T* data_ = nullptr;

public:
    ListBaseTIterator(T* data)
        : data_(data)
    {
    }

    ListBaseTIterator& operator++()
    {
        data_ = reinterpret_cast<T*>((reinterpret_cast<const Link*>(data_))->next);
        return *this;
    }

    ListBaseTIterator operator++(int)
    {
        ListBaseTIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    ListBaseTIterator& operator--()
    {
        data_ = reinterpret_cast<T*>((reinterpret_cast<const Link*>(data_))->prev);
        return *this;
    }

    ListBaseTIterator operator--(int)
    {
        ListBaseTIterator tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(const ListBaseTIterator& a,
                           const ListBaseTIterator& b)
    {
        return a.data_ == b.data_;
    }

    friend bool operator!=(const ListBaseTIterator& a,
                           const ListBaseTIterator& b)
    {
        return a.data_ != b.data_;
    }

    T& operator*() const { return *data_; }
};

template <typename T>
struct ListBaseEnumerateWrapper;

template <typename T>
struct ListBaseMutableWrapper;

template <typename T>
struct ListBaseBackwardWrapper;

template <typename T>
struct ListBaseMutableBackwardWrapper;

template <typename T>
struct ListBaseT : public ListBase
{
    ListBaseTIterator<T>              begin() const { return ListBaseTIterator<T>{static_cast<T*>(this->first)}; }

    ListBaseTIterator<T>              end() const { return ListBaseTIterator<T>{nullptr}; }

    ListBaseEnumerateWrapper<T>       enumerate() { return {this->first}; }

    ListBaseEnumerateWrapper<const T> enumerate() const { return {this->first}; }

    ListBaseMutableWrapper<T>         items_mutable() { return {this->first}; }

    ListBaseBackwardWrapper<T>        items_reversed() { return {this->last}; }

    ListBaseBackwardWrapper<const T>  items_reversed() const { return {this->last}; }

    ListBaseMutableBackwardWrapper<T> items_reversed_mutable() { return {this->last}; }

    template <typename OtherT>
    const ListBaseT<OtherT>& cast() const
    {
        return *reinterpret_cast<const ListBaseT<OtherT>*>(this);
    }

    template <typename OtherT>
    ListBaseT<OtherT>& cast()
    {
        return *reinterpret_cast<ListBaseT<OtherT>*>(this);
    }
};

VKE_INLINE bool VKE_listbase_is_single(const ListBase* lb)
{
    return (lb->first && lb->first == lb->last);
}

#endif
} // namespace vektor
