#include "MEM_gaurdalloc.h"
#include "../clog/CLG_log.h"
#include "MEM_function_pointers.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

CLG_LOGREF_DECLARE_GLOBAL(LOG_MEM, "MEM");

#define MEM_MAGIC 0xDEADBEEF

namespace mem_guarded::internal {

struct MemHead {
  uint32_t magic;
  size_t size;
  size_t raw_offset; /* bytes from raw malloc ptr to this MemHead */
  const char *name;
};

static size_t total_memory_in_use = 0;

void (*mem_freeN_ex)(void *vmemh, DestructorType /*destructor_type*/) =
    [](void *vmemh, DestructorType /*destructor_type*/) {
      if (!vmemh)
        return;

      auto *head = (MemHead *)((char *)vmemh - sizeof(MemHead));
      if (head->magic != MEM_MAGIC) {
        CLOG_ERROR(LOG_MEM, "Memory corruption detected on free: invalid magic head byte!");
        std::abort();
      }

      auto *tail = (uint32_t *)((char *)vmemh + head->size);
      if (*tail != MEM_MAGIC) {
        CLOG_ERROR(LOG_MEM, "Memory corruption detected on free: invalid magic tail byte!");
        std::abort();
      }

      total_memory_in_use -= head->size;

      /* Recover the original raw pointer that was returned by malloc. */
      void *raw = (char *)head - head->raw_offset;
      free(raw);
    };

void *(*mem_mallocN_aligned_ex)(size_t len,
                                size_t alignment,
                                const char *name,
                                DestructorType /*destructor_type*/) =
    [](size_t len,
       size_t alignment,
       const char *name,
       DestructorType /*destructor_type*/) -> void * {
  /* we use the guarded layout (MemHead + user data + magic tail) so that
   * mem_freeN_ex can validate and free every pointer uniformly.
   *
   * For over-aligned requests we over-allocate enough room to align the
   * user-data region manually inside a plain malloc'd block. */
  size_t head_size = sizeof(MemHead);
  size_t tail_size = sizeof(uint32_t);

  /* Extra padding needed so the user pointer can be aligned. */
  size_t align_pad = (alignment > alignof(std::max_align_t)) ? (alignment - 1) : 0;
  size_t total_size = head_size + align_pad + len + tail_size;

  void *raw = malloc(total_size);
  if (!raw)
    return nullptr;

  /* User pointer: the byte right after the MemHead, rounded up to alignment. */
  char *user_ptr = (char *)raw + head_size;
  if (align_pad) {
    auto addr = (uintptr_t)user_ptr;
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    user_ptr = (char *)aligned;
  }

  /* Write the MemHead immediately before the user pointer. */
  auto *head = (MemHead *)(user_ptr - head_size);
  head->magic = MEM_MAGIC;
  head->size = len;
  head->raw_offset = (size_t)((char *)head - (char *)raw);
  head->name = name;

  /* Write the magic tail immediately after the user data. */
  auto *tail = (uint32_t *)(user_ptr + len);
  *tail = MEM_MAGIC;

  total_memory_in_use += len;
  return user_ptr;
};

void *(*mem_mallocN)(size_t len, const char *str) = [](size_t len, const char *str) -> void * {
  return mem_mallocN_aligned_ex(len, alignof(std::max_align_t), str, DestructorType::Trivial);
};

FrameAllocator g_frame_allocator{};

void FrameAllocator::init()
{
  assert(buffer == nullptr && "FrameAllocator::init() called more than once");
  capacity = MEM_FRAME_BUFFER_SIZE;
  buffer   = static_cast<uint8_t *>(std::malloc(capacity));
  if (!buffer) {
    CLOG_ERROR(LOG_MEM, "FrameAllocator: failed to allocate 16 MB frame slab!");
    std::abort();
  }
  cursor.store(0, std::memory_order_relaxed);
  CLOG_INFO(LOG_MEM, "FrameAllocator: 16 MB slab initialised at %p", (void *)buffer);
}

void *FrameAllocator::alloc(size_t size, size_t alignment)
{
  if (!size)
    return nullptr;

  /* Bump the cursor with alignment padding using a CAS loop so the allocator
   * is safe to call from multiple threads concurrently within one frame. */
  size_t old_cursor = cursor.load(std::memory_order_relaxed);
  size_t new_cursor;
  void *result;

  do {
    /* Round up old_cursor to the requested alignment. */
    size_t aligned = (old_cursor + alignment - 1) & ~(alignment - 1);
    new_cursor     = aligned + size;

    if (new_cursor > capacity) {
      CLOG_ERROR(LOG_MEM,
                 "FrameAllocator: slab exhausted! Requested %zu bytes, "
                 "%zu / %zu bytes already used.",
                 size,
                 old_cursor,
                 capacity);
      return nullptr;
    }

    result = buffer + aligned;
  } while (!cursor.compare_exchange_weak(
      old_cursor, new_cursor, std::memory_order_acq_rel, std::memory_order_relaxed));

  return result;
}

void FrameAllocator::end_frame()
{
  /* Stamp the slab with a recognisable pattern in debug to catch use-after-end_frame bugs. */
#ifndef NDEBUG
  size_t used = cursor.load(std::memory_order_relaxed);
  if (used)
    std::memset(buffer, 0xCD, used); /* 0xCD = "clean dead" sentinel */
#endif
  cursor.store(0, std::memory_order_release);
}

size_t FrameAllocator::bytes_used() const
{
  return cursor.load(std::memory_order_relaxed);
}

void FrameAllocator::shutdown()
{
  std::free(buffer);
  buffer   = nullptr;
  capacity = 0;
  cursor.store(0, std::memory_order_relaxed);
}

/* Public function-pointer hooks (match the extern declarations in MEM_function_pointers.h). */
void *(*mem_frame_alloc)(size_t size) = [](size_t size) -> void * {
  return g_frame_allocator.alloc(size);
};

void (*mem_frame_end)() = []() {
  g_frame_allocator.end_frame();
};

}  // namespace mem_guarded::internal

namespace mem {

void *(*MEM_new_array_zeroed_aligned)(size_t len, size_t size, size_t alignment, const char *str) =
    [](size_t len, size_t size, size_t alignment, const char *str) -> void * {
  size_t total_size = len * size;
  void *ptr = mem_guarded::internal::mem_mallocN_aligned_ex(
      total_size, alignment, str, mem_guarded::internal::DestructorType::Trivial);
  if (ptr) {
    std::memset(ptr, 0, total_size);
  }
  return ptr;
};

void *MEM_new_array_uninitialized(size_t len, size_t size, const char *str)
{
  return mem_guarded::internal::mem_mallocN_aligned_ex(
      len * size, alignof(std::max_align_t), str, mem_guarded::internal::DestructorType::Trivial);
}

void *(*MEM_realloc_uninitialized_id)(void *vmemh, size_t len, const char * /*str*/) =
    [](void *vmemh, size_t len, const char * /*str*/) -> void * {
  if (!vmemh) {
    /* Behave like realloc(nullptr, len): allocate a new guarded block. */
    return mem_guarded::internal::mem_mallocN(len, "MEM_realloc");
  }

  /* vmemh points to the user region; the MemHead sits right before it. */
  auto *old_head = (mem_guarded::internal::MemHead *)((char *)vmemh -
                                                      sizeof(mem_guarded::internal::MemHead));
  if (old_head->magic != MEM_MAGIC) {
    CLOG_ERROR(LOG_MEM,
               "MEM_realloc: invalid magic – pointer was not allocated by this allocator!");
    std::abort();
  }

  size_t old_len = old_head->size;

  /* Recover the original malloc pointer using the stored offset. */
  void *old_raw = (char *)old_head - old_head->raw_offset;
  size_t total_size = old_head->raw_offset +
                      sizeof(mem_guarded::internal::MemHead) + len + sizeof(uint32_t);

  void *raw = std::realloc(old_raw, total_size);
  if (!raw)
    return nullptr;

  /* After realloc, reconstruct head at the same raw_offset within the new block. */
  auto reuse_offset = (size_t)((char *)old_head - (char *)old_raw); /* == old raw_offset */
  auto *new_head = (mem_guarded::internal::MemHead *)((char *)raw + reuse_offset);
  new_head->magic = MEM_MAGIC;
  new_head->size = len;
  new_head->raw_offset = reuse_offset;
  /* name pointer is still valid (it's a string literal / static lifetime). */

  char *user_ptr = (char *)new_head + sizeof(mem_guarded::internal::MemHead);

  /* Write the new tail. */
  auto *tail = (uint32_t *)(user_ptr + len);
  *tail = MEM_MAGIC;

  mem_guarded::internal::total_memory_in_use = mem_guarded::internal::total_memory_in_use -
                                               old_len + len;

  return user_ptr;
};

size_t (*MEM_get_memory_in_use)() = []() -> size_t {
  return mem_guarded::internal::total_memory_in_use;
};

}  // namespace mem
