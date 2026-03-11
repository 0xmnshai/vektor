#pragma once

#include <utility>

#include "../../source/runtime/lib/VLI_complier_attrs.h"
#include "MEM_function_pointers.h"

namespace mem {
template<typename T, typename... Args>
inline T *MEM_new(const char *allocation_name, Args &&...args)
{
  void *buffer = mem_guarded::internal::mem_mallocN_aligned_ex(
      sizeof(T),
      alignof(T),
      allocation_name,
      std::is_trivially_destructible_v<T> ? mem_guarded::internal::DestructorType::Trivial :
                                            mem_guarded::internal::DestructorType::NonTrivial);
  return new (buffer) T(std::forward<Args>(args)...);
}

extern void *(*MEM_new_array_zeroed_aligned)(
    size_t len,
    size_t size,
    size_t alignment,
    const char *str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1, 2)
    ATTR_NONNULL(4);

void *MEM_new_array_uninitialized(size_t len,
                                  size_t size,
                                  const char *str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT
    ATTR_ALLOC_SIZE(1, 2) ATTR_NONNULL(3);

template<typename T> inline void MEM_delete(const T *ptr)
{
  static_assert(
      !std::is_void_v<T>,
      "MEM_delete on a void pointer is not possible, `static_cast` it to the correct type");
  if (ptr == nullptr) {
    return;
  }
  const void *complete_ptr = [ptr]() {
    if constexpr (std::is_polymorphic_v<T>) {
      return dynamic_cast<const void *>(ptr);
    }
    else {
      return static_cast<const void *>(ptr);
    }
  }();
  if constexpr (!std::is_trivially_destructible_v<T>) {
    ptr->~T();
  }
  mem_guarded::internal::mem_freeN_ex(const_cast<void *>(complete_ptr),
                                      mem_guarded::internal::DestructorType::NonTrivial);
}

extern void *(*MEM_realloc_uninitialized_id)(
    void *vmemh, size_t len, const char *str) /* ATTR_MALLOC */ ATTR_WARN_UNUSED_RESULT
    ATTR_ALLOC_SIZE(2);

extern size_t (*MEM_get_memory_in_use)();

#define MEM_realloc_uninitialized(vmemh, len) MEM_realloc_uninitialized_id(vmemh, len, __func__)

#define MEM_SAFE_DELETE(v) \
  do { \
    if (v) { \
      MEM_delete(v); \
      (v) = nullptr; \
    } \
  } while (0)

template<typename T> inline T *MEM_new_zeroed(const char *allocation_name)
{
  static_assert(std::is_trivial_v<T>, "For non-trivial types, MEM_new must be used.");
  return static_cast<T *>(MEM_new_array_zeroed_aligned(1, sizeof(T), alignof(T), allocation_name));
}

#define MEM_mallocN(len, str) mem_guarded::internal::mem_mallocN(len, str)

#define MEM_freeN(vmemh) \
  mem_guarded::internal::mem_freeN_ex(vmemh, mem_guarded::internal::DestructorType::Trivial)

#define MEM_freeN_ex(vmemh, destructor_type) \
  mem_guarded::internal::mem_freeN_ex(vmemh, destructor_type)

#define MEM_FRAME_INIT() mem_guarded::internal::g_frame_allocator.init()

/** Release the slab at application shutdown. */
#define MEM_FRAME_SHUTDOWN() mem_guarded::internal::g_frame_allocator.shutdown()

/**
 * Allocate `size` bytes from the frame slab with default (max) alignment.
 * The pointer is valid until MEM_frame_end() is called at the end of the frame.
 * Do NOT call MEM_freeN on it – the whole slab is rewound as one unit.
 */
#define MEM_frame_alloc(size) mem_guarded::internal::mem_frame_alloc(size)

/**
 * Allocate `size` bytes from the frame slab with an explicit `alignment`.
 * alignment must be a power-of-two.
 */
#define MEM_frame_alloc_aligned(size, alignment) \
  mem_guarded::internal::g_frame_allocator.alloc(size, alignment)

/**
 * Rewind the frame slab.  Call exactly once per frame after all frame work
 * is complete (e.g. after present / swap-buffers).  All previous
 * MEM_frame_alloc pointers become invalid after this call.
 */
#define MEM_frame_end() mem_guarded::internal::mem_frame_end()

/** Query how many slab bytes are currently live (useful for profiling). */
#define MEM_frame_bytes_used() mem_guarded::internal::g_frame_allocator.bytes_used()

}  // namespace mem

/*
 * // --- Application / renderer startup ---
 * MEM_FRAME_INIT();   // allocates the 16 MB slab once
 *
 * // --- Inside the render / game loop (called ~60 times per second) ---
 * void render_frame()
 * {
 *     // Build a temporary label – no heap allocation, no free needed.
 *     char *label = static_cast<char *>(MEM_frame_alloc(128));
 *     std::snprintf(label, 128, "Frame #%u | dt=%.3f ms", frame_index, delta_ms);
 *     draw_hud_text(label);   // used this frame only
 *
 *     // Scratch matrix for a one-shot transform calculation.
 *     float *mat = static_cast<float *>(MEM_frame_alloc_aligned(16 * sizeof(float), 16));
 *     build_projection_matrix(mat, fov, aspect, z_near, z_far);
 *     upload_uniform("u_proj", mat, 16);
 *
 *     // ... rest of the frame ...
 *
 *     // Optional: profile how much slab we consumed this frame.
 *     // size_t used = MEM_frame_bytes_used();
 *
 *     // Must be called LAST – invalidates all pointers from this frame.
 *     MEM_frame_end();
 * }
 *
 * // --- Application shutdown ---
 * MEM_FRAME_SHUTDOWN();  // frees the 16 MB slab
 */
