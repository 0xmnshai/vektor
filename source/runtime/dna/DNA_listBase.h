
#pragma once

#include <cstddef>

#include "../vklib/VKE_compiler_attrs.h"
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

VKE_INLINE bool VKE_listbase_is_single(const ListBase* lb)
{
    return (lb->first && lb->first == lb->last);
}

VKE_INLINE void BLI_listbase_clear(ListBase* lb)
{
    lb->first = lb->last = static_cast<void*>(nullptr);
}

int   BLI_findindex(const ListBase* listbase,
                    const void*     vlink) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL(1);

void* BLI_pophead(ListBase* listbase) ATTR_NONNULL(1);

void* BLI_findptr(const ListBase* listbase,
                  const void*     ptr,
                  int             offset) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL(1);

void* BLI_listbase_bytes_find(const ListBase* listbase,
                              const void*     bytes,
                              size_t          bytes_size,
                              int             offset) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL(1,
                                                                               2);

void  BLI_addhead(ListBase* listbase,
                  void*     vlink) ATTR_NONNULL(1);

#endif
} // namespace vektor
