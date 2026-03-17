#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

#include "../../source/runtime/lib/VLI_complier_attrs.h"

namespace mem_guarded::internal {

enum class DestructorType {
    /** Allocation has a trivial destructor. */
    Trivial,
    /** Allocation has a non-trivial destructor that should be called when freeing. */
    NonTrivial,
};

extern void (*mem_freeN_ex)(void *vmemh, DestructorType destructor_type);
extern void *(*mem_mallocN_aligned_ex)(size_t len,
                                       size_t alignment,
                                       const char *str,
                                       DestructorType destructor_type);

extern void *(*mem_mallocN)(size_t len, const char *str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT
ATTR_ALLOC_SIZE(1) ATTR_NONNULL(2);

/*
 * Frame Allocator
 *
 * A fixed-size linear/bump allocator backed by a 16 MB slab that is committed
 * at startup and rewound (cursor reset to 0) at the end of every frame via
 * MEM_frame_end().  Allocations are O(1) and produce NO heap traffic – ideal
 * for short-lived temporaries (string scratch buffers, per-frame math matrices,
 * transient work arrays, etc.) that must not out-live the current frame.
 *
 * Thread-safety: the internal cursor is advanced with std::atomic so concurrent
 * allocations from multiple threads within the same frame are safe.  end_frame()
 * must be called from a single "owner" context after all worker threads have
 * finished for the frame.
 * */
constexpr size_t MEM_FRAME_BUFFER_SIZE = 16u * 1024u * 1024u; /* 16 MB */

struct FrameAllocator {
    uint8_t *buffer;                   /**< Base of the 16 MB slab (heap-allocated once). */
    std::atomic<size_t> cursor;        /**< Byte offset of the next free byte. */
    size_t capacity;                   /**< Total slab size in bytes (= MEM_FRAME_BUFFER_SIZE). */
    
    /** Initialise the allocator.  Called once at program / renderer start-up. */
    void init();
    
    /**
     * Bump-allocate `size` bytes aligned to `alignment`.
     * Returns nullptr when the slab is exhausted (caller must handle gracefully).
     */
    void *alloc(size_t size, size_t alignment = alignof(std::max_align_t)) ATTR_WARN_UNUSED_RESULT;
    
    /**
     * Reset the cursor to 0, making the entire slab available again.
     * Call once per frame after all frame work is complete.
     */
    void end_frame();
    
    /** Return how many bytes are currently live in the slab. */
    size_t bytes_used() const;
    
    /** Free the underlying slab.  Call at application shutdown. */
    void shutdown();
};

/** The singleton frame allocator instance. */
extern FrameAllocator g_frame_allocator;

/**
 * Allocate `size` bytes from the frame slab with default alignment.
 * Equivalent to g_frame_allocator.alloc(size).
 */
extern void *(*mem_frame_alloc)(size_t size) ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1);

/**
 * Flush / rewind the frame allocator.  Must be called once per frame (e.g.
 * at the very end of the render loop) AFTER all frame allocations are no
 * longer referenced.
 */
extern void (*mem_frame_end)();

}  // namespace mem_guarded::internal
