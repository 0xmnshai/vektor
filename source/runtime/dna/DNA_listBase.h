
#pragma once

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
struct ListBaseTIterator;

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

#endif
} // namespace vektor
