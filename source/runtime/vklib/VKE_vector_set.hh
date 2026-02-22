#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "VKE_memory_util.hh"
#include "VKE_span.hh"

namespace vektor
{

template <typename T>
struct DefaultHash
{
    uint64_t operator()(const T& value) const { return std::hash<T>{}(value); }
};

template <typename T>
struct DefaultEquality
{
    bool operator()(const T& a,
                    const T& b) const
    {
        return std::equal_to<T>{}(a, b);
    }
};

struct DefaultProbingStrategy
{
    void next(uint64_t& slot_index,
              uint64_t  hash,
              uint64_t  mask,
              uint64_t  iteration) const
    {
        slot_index = (slot_index + 1) & mask;
    }

    uint64_t initial(uint64_t hash,
                     uint64_t mask) const
    {
        return hash & mask;
    }
};

template <typename Key>
class VectorSetSlot
{
public:
    VectorSetSlot()
        : index_(-1)
        , hash_(0)
    {
    }

    bool is_empty() const { return index_ == -1; }
    bool is_occupied() const { return index_ >= 0; }
    bool is_removed() const { return index_ == -2; }

    void occupy(int64_t  index,
                uint64_t hash)
    {
        index_ = index;
        hash_  = hash;
    }

    void     remove() { index_ = -2; }

    int64_t  index() const { return index_; }
    uint64_t hash() const { return hash_; }

    template <typename IsEqual>
    bool contains(const Key& key,
                  IsEqual&   is_equal,
                  uint64_t   hash,
                  const Key* keys) const
    {
        if (hash_ != hash)
        {
            return false;
        }
        return is_equal(keys[index_], key);
    }

    // Helper for specific use cases (like remove_key_internal)
    void update_index(int64_t new_index) { index_ = new_index; }
    bool has_index(int64_t index) const { return index_ == index; }

private:
    int64_t  index_;
    uint64_t hash_;
};

template <typename Key,
          int64_t InlineBufferCapacity = default_inline_buffer_capacity(sizeof(Key)),
          typename ProbingStrategy     = DefaultProbingStrategy,
          typename Hash                = DefaultHash<Key>,
          typename IsEqual             = DefaultEquality<Key>,
          typename Slot                = VectorSetSlot<Key>>
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

private:
    int64_t           removed_slots_              = 0;
    int64_t           occupied_and_removed_slots_ = 0;
    int64_t           usable_slots_               = 0;
    uint64_t          slot_mask_                  = 0;

    Hash              hash_;
    IsEqual           is_equal_;

    // Use std::vector for slots. always allocated on heap.
    std::vector<Slot> slots_;

    // Inline buffer for keys.
    // Using aligned_storage to avoid default construction of keys.
    using InlineBuffer =
        typename std::aligned_storage<sizeof(Key), alignof(Key)>::type[InlineBufferCapacity > 0 ? InlineBufferCapacity
                                                                                                : 1];
    InlineBuffer           inline_buffer_;

    // Pointer to keys array.
    Key*                   keys_           = nullptr;

    static constexpr float MAX_LOAD_FACTOR = 0.5f;

public:
    VectorSet()
        : slots_(1) // Initial size 1
    {
        keys_ = reinterpret_cast<Key*>(&inline_buffer_);
    }

    VectorSet(Hash    hash,
              IsEqual is_equal)
        : VectorSet()
    {
        hash_     = std::move(hash);
        is_equal_ = std::move(is_equal);
    }

    // Copy constructor
    VectorSet(const VectorSet& other)
        : VectorSet(other.hash_,
                    other.is_equal_)
    {
        reserve(other.size());
        for (const auto& key : other)
        {
            add(key);
        }
    }

    // Move constructor
    VectorSet(VectorSet&& other) noexcept
        : removed_slots_(other.removed_slots_)
        , occupied_and_removed_slots_(other.occupied_and_removed_slots_)
        , usable_slots_(other.usable_slots_)
        , slot_mask_(other.slot_mask_)
        , hash_(std::move(other.hash_))
        , is_equal_(std::move(other.is_equal_))
        , slots_(std::move(other.slots_))
    {
        if (other.is_inline())
        {
            // Move inline keys
            keys_ = reinterpret_cast<Key*>(&inline_buffer_);
            std::uninitialized_move_n(other.keys_, other.size(), keys_);
        }
        else
        {
            keys_ = other.keys_;
        }

        // Reset other
        other.keys_                       = reinterpret_cast<Key*>(&other.inline_buffer_);
        other.removed_slots_              = 0;
        other.occupied_and_removed_slots_ = 0;
        other.usable_slots_               = 0;
        other.slot_mask_                  = 0;
        other.slots_.resize(1); // Reset to valid state
    }

    // Copy assignment
    VectorSet& operator=(const VectorSet& other)
    {
        if (this == &other)
            return *this;
        clear();
        hash_     = other.hash_;
        is_equal_ = other.is_equal_;
        reserve(other.size());
        for (const auto& key : other)
        {
            add(key);
        }
        return *this;
    }

    // Move assignment
    VectorSet& operator=(VectorSet&& other) noexcept
    {
        if (this == &other)
            return *this;
        clear();
        destruct_keys(); // clean current
        if (!is_inline() && keys_)
        {
            std::free(keys_);
        }

        removed_slots_              = other.removed_slots_;
        occupied_and_removed_slots_ = other.occupied_and_removed_slots_;
        usable_slots_               = other.usable_slots_;
        slot_mask_                  = other.slot_mask_;
        hash_                       = std::move(other.hash_);
        is_equal_                   = std::move(other.is_equal_);
        slots_                      = std::move(other.slots_);

        if (other.is_inline())
        {
            keys_ = reinterpret_cast<Key*>(&inline_buffer_);
            std::uninitialized_move_n(other.keys_, other.size(), keys_);
        }
        else
        {
            keys_ = other.keys_;
        }

        // Reset other
        other.keys_                       = reinterpret_cast<Key*>(&other.inline_buffer_);
        other.removed_slots_              = 0;
        other.occupied_and_removed_slots_ = 0;
        other.usable_slots_               = 0;
        other.slot_mask_                  = 0;
        other.slots_.clear();
        other.slots_.resize(1);

        return *this;
    }

    ~VectorSet()
    {
        destruct_keys();
        if (!is_inline() && keys_)
        {
            // Use free because we used malloc/aligned_alloc (simulated via operator new here for simplicity but let's
            // stick to standard ways) Actually, let's use ::operator delete to match ::operator new
            ::operator delete(keys_);
        }
    }

    int64_t          size() const { return occupied_and_removed_slots_ - removed_slots_; }
    bool             is_empty() const { return size() == 0; }

    bool             is_inline() const { return keys_ == reinterpret_cast<const Key*>(&inline_buffer_); }

    const Key*       begin() const { return keys_; }
    const Key*       end() const { return keys_ + size(); }

    vklib::Span<Key> as_span() const { return vklib::Span<Key>(keys_, size()); }
                     operator vklib::Span<Key>() const { return as_span(); }

    const Key&       operator[](int64_t index) const
    {
        assert(index >= 0 && index < size());
        return keys_[index];
    }

    bool add(const Key& key) { return add_impl(key, hash_(key)); }
    bool add(Key&& key) { return add_impl(std::move(key), hash_(key)); }

    bool contains(const Key& key) const
    {
        uint64_t hash  = hash_(key);
        int64_t  index = find_slot(key, hash);
        return index >= 0 && slots_[index].is_occupied();
    }

    int64_t index_of(const Key& key) const
    {
        uint64_t hash     = hash_(key);
        int64_t  slot_idx = find_slot(key, hash);
        if (slot_idx >= 0 && slots_[slot_idx].is_occupied())
        {
            return slots_[slot_idx].index();
        }
        return -1;
    }

    bool remove(const Key& key)
    {
        uint64_t hash     = hash_(key);
        int64_t  slot_idx = find_slot(key, hash);
        if (slot_idx >= 0 && slots_[slot_idx].is_occupied())
        {
            remove_key_internal(slots_[slot_idx]);
            return true;
        }
        return false;
    }

    void reserve(int64_t capacity)
    {
        if (usable_slots_ < capacity)
        {
            realloc_and_reinsert(capacity);
        }
    }

    void clear()
    {
        destruct_keys();
        // Reset slots but keep capacity roughly? Or just reset logic.
        // For simplicity, let's reset to initial state.
        std::fill(slots_.begin(), slots_.end(), Slot()); // Clear slots
        removed_slots_              = 0;
        occupied_and_removed_slots_ = 0;
        // usable_slots kept same mostly
    }

private:
    void destruct_keys()
    {
        if (keys_)
        {
            for (int64_t i = 0; i < size(); ++i)
            {
                keys_[i].~Key();
            }
        }
    }

    template <typename ForwardKey>
    bool add_impl(ForwardKey&& key,
                  uint64_t     hash)
    {
        ensure_can_add();

        // Find empty slot or existing
        ProbingStrategy strategy;
        uint64_t        mask     = slots_.size() - 1;
        uint64_t        slot_idx = strategy.initial(hash, mask);

        for (uint64_t i = 0;; ++i)
        {
            Slot& slot = slots_[slot_idx];

            if (slot.is_empty())
            {
                // Not found, insert here
                int64_t index = size();
                new (keys_ + index) Key(std::forward<ForwardKey>(key));
                slot.occupy(index, hash);
                occupied_and_removed_slots_++;
                return true;
            }

            if (slot.contains(key, is_equal_, hash, keys_))
            {
                return false; // Already exists
            }

            strategy.next(slot_idx, hash, mask, i);
        }
    }

    void ensure_can_add()
    {
        if (occupied_and_removed_slots_ >= usable_slots_)
        {
            realloc_and_reinsert(size() + 1);
        }
    }

    void realloc_and_reinsert(int64_t min_capacity)
    {
        // Calculate new size
        int64_t new_capacity = std::max<int64_t>(min_capacity, size() * 2);
        if (new_capacity < 4)
            new_capacity = 4; // Min size

        // Power of 2
        int64_t slots_size = 4;
        while (slots_size * MAX_LOAD_FACTOR < new_capacity)
        {
            slots_size *= 2;
        }

        // Realloc keys if needed
        Key* new_keys   = keys_;
        bool moved_keys = false;

        if (new_capacity > InlineBufferCapacity)
        {
            if (is_inline() || usable_slots_ < new_capacity)
            {
                new_keys = static_cast<Key*>(::operator new(sizeof(Key) * new_capacity));
                // Move existing
                if (size() > 0)
                {
                    std::uninitialized_move_n(keys_, size(), new_keys);
                    destruct_keys(); // Destruct old ones (moved from)
                }
                if (!is_inline())
                {
                    ::operator delete(keys_);
                }
                moved_keys = true;
            }
        }

        Key* old_keys_ptr = keys_;        // Keep track for fixup during rehash? No need if we moved first.
        keys_             = new_keys;     // Update keys pointer
        usable_slots_     = new_capacity; // Strictly this is key capacity, but logic usually ties them.

        // Rebuild slots
        std::vector<Slot> new_slots(slots_size);
        uint64_t          mask = slots_size - 1;
        ProbingStrategy   strategy;

        for (int64_t i = 0; i < size(); ++i)
        {
            const Key& key      = keys_[i];
            uint64_t   hash     = hash_(key);
            uint64_t   slot_idx = strategy.initial(hash, mask);

            for (uint64_t j = 0;; ++j)
            {
                if (new_slots[slot_idx].is_empty())
                {
                    new_slots[slot_idx].occupy(i, hash);
                    break;
                }
                strategy.next(slot_idx, hash, mask, j);
            }
        }

        slots_                      = std::move(new_slots);
        slot_mask_                  = mask;
        usable_slots_               = int64_t(slots_.size() * MAX_LOAD_FACTOR);

        // Reset removed count as we compacted/rehashed (actually we didn't compact keys array, just slots)
        // If we want to compact keys array (remove holes from removed slots), we should do it here.
        // The original implementation does not seem to compact keys on simple grow, but realloc_and_reinsert DOES
        // compact because it iterates over existing valid keys? Wait, original: add_after_grow takes old_slot.index().
        // If we have holes, simple move of keys array preserves holes.
        // Current implementation: size() is (occupied + removed) - removed.
        // My implementation above: I moved `size()` keys? No, `size()` is logically valid keys, but
        // `occupied_and_removed_slots_` is the index range.

        // Correction: keys are stored contiguously. When removing, we swap with last. So there are NO holes in keys
        // array. `removed_slots_` just tracks how many times we did swap-remove to check if we should shrink/rehash
        // triggered by `occupied_and_removed_slots`. So keys are always [0, size()).

        removed_slots_              = 0;
        occupied_and_removed_slots_ = size();
    }

    int64_t find_slot(const Key& key,
                      uint64_t   hash) const
    {
        if (slots_.empty())
            return -1;
        ProbingStrategy strategy;
        uint64_t        mask     = slots_.size() - 1;
        uint64_t        slot_idx = strategy.initial(hash, mask);

        for (uint64_t i = 0;; ++i)
        {
            const Slot& slot = slots_[slot_idx];
            if (slot.is_empty())
                return -1;
            if (slot.contains(key, is_equal_, hash, keys_))
                return slot_idx;
            strategy.next(slot_idx, hash, mask, i);
        }
    }

    void remove_key_internal(Slot& slot)
    {
        int64_t index_to_remove = slot.index();
        int64_t last_index      = size() - 1;

        if (index_to_remove != last_index)
        {
            // Swap with last
            keys_[index_to_remove] = std::move(keys_[last_index]);
            update_slot_index(keys_[index_to_remove], last_index, index_to_remove);
        }

        keys_[last_index].~Key();
        slot.remove();
        removed_slots_++;
    }

    void update_slot_index(const Key& key,
                           int64_t    old_index,
                           int64_t    new_index)
    {
        uint64_t        hash = hash_(key);
        ProbingStrategy strategy;
        uint64_t        mask     = slots_.size() - 1;
        uint64_t        slot_idx = strategy.initial(hash, mask);

        for (uint64_t i = 0;; ++i)
        {
            Slot& slot = slots_[slot_idx];
            // If we hit empty, something is wrong because the key SHOULD be there.
            // But strict probing says we stop at empty.
            if (slot.is_empty())
                return;

            if (slot.has_index(old_index))
            {
                slot.update_index(new_index);
                return;
            }
            strategy.next(slot_idx, hash, mask, i);
        }
    }
};

template <typename T>
inline uint64_t get_default_hash(const T& value)
{
    return DefaultHash<T>{}(value);
}

template <typename T, typename GetIDFn>
struct CustomIDHash
{
    using CustomIDType = decltype(GetIDFn{}(std::declval<T>()));

    uint64_t operator()(const T& value) const { return get_default_hash(GetIDFn{}(value)); }
    uint64_t operator()(const CustomIDType& value) const { return get_default_hash(value); }
};

template <typename T, typename GetIDFn>
struct CustomIDEqual
{
    using CustomIDType = decltype(GetIDFn{}(std::declval<T>()));

    bool operator()(const T& a,
                    const T& b) const
    {
        return GetIDFn{}(a) == GetIDFn{}(b);
    }
    bool operator()(const CustomIDType& a,
                    const T&            b) const
    {
        return a == GetIDFn{}(b);
    }
    bool operator()(const T&            a,
                    const CustomIDType& b) const
    {
        return GetIDFn{}(a) == b;
    }
};

template <typename T, typename GetIDFn, int64_t InlineBufferCapacity = 4>
using CustomIDVectorSet =
    VectorSet<T, InlineBufferCapacity, DefaultProbingStrategy, CustomIDHash<T, GetIDFn>, CustomIDEqual<T, GetIDFn>>;

} // namespace vektor