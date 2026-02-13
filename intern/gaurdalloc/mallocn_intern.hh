

#ifdef __cplusplus
#include <cstddef>
#else
#include <stdbool.h>
#include <stddef.h>
#endif

#define SIZET_FORMAT "%zu"
#define SIZET_ARG(a) ((size_t)(a))

#define SIZET_ALIGN_4(len) ((len + 3) & ~(size_t)3)

#include <stdlib.h>

#define IS_POW2(a) (((a) & ((a) - 1)) == 0)

#define MEMHEAD_ALIGN_PADDING(alignment) ((size_t)alignment - (sizeof(MemHeadAligned) % (size_t)alignment))

#define MEMHEAD_REAL_PTR(memh) ((char*)memh - MEMHEAD_ALIGN_PADDING(memh->alignment))

#include "mallocn_defines.hh"
#ifdef __cplusplus
#include "malloc_function_pointers.hh"
#endif

#define ALIGNED_MALLOC_MINIMUM_ALIGNMENT sizeof(void*)

void*       aligned_malloc(size_t size,
                           size_t alignment);
void        aligned_free(void* ptr);

extern bool leak_detector_has_run;
extern char free_after_leak_detection_message[];

void        memory_usage_init(void);
void        memory_usage_block_alloc(size_t size);
void        memory_usage_block_free(size_t size);
size_t      memory_usage_block_num(void);
size_t      memory_usage_current(void);
size_t      memory_usage_peak(void);
void        memory_usage_peak_reset(void);

extern void (*mem_clearmemlist)(void);

#define LIKELY(x) (x)
#define UNLIKELY(x) (x)

size_t MEM_lockfree_allocN_len(const void* vmemh) ATTR_WARN_UNUSED_RESULT;
#ifdef __cplusplus
void MEM_lockfree_freeN(void*                                         vmemh,
                        vektor::mem_guarded::internal::DestructorType destructor_type);
#endif
void* MEM_lockfree_dupallocN(const void* vmemh) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT;
void* MEM_lockfree_reallocN_id(void*       vmemh,
                               size_t      len,
                               const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(2);
void* MEM_lockfree_recallocN_id(void*       vmemh,
                                size_t      len,
                                const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(2);
void* MEM_lockfree_callocN(size_t      len,
                           const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(2);
void* MEM_lockfree_calloc_arrayN(size_t      len,
                                 size_t      size,
                                 const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                      2)
    ATTR_NONNULL(3);
void* MEM_lockfree_mallocN(size_t      len,
                           const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(2);
void* MEM_lockfree_malloc_arrayN(size_t      len,
                                 size_t      size,
                                 const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                      2)
    ATTR_NONNULL(3);
#ifdef __cplusplus
void* MEM_lockfree_mallocN_aligned(size_t                                        len,
                                   size_t                                        alignment,
                                   const char*                                   str,
                                   vektor::mem_guarded::internal::DestructorType destructor_type)
    ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(3);
#endif
void* MEM_lockfree_malloc_arrayN_aligned(size_t      len,
                                         size_t      size,
                                         size_t      alignment,
                                         const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                              2)
    ATTR_NONNULL(4);
void* MEM_lockfree_calloc_arrayN_aligned(size_t      len,
                                         size_t      size,
                                         size_t      alignment,
                                         const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                              2)
    ATTR_NONNULL(4);

void         MEM_lockfree_printmemlist_pydict(void);
void         MEM_lockfree_printmemlist(void);
void         MEM_lockfree_callbackmemlist(void (*func)(void*));
void         MEM_lockfree_printmemlist_stats(void);
void         MEM_lockfree_set_error_callback(void (*func)(const char*));
bool         MEM_lockfree_consistency_check(void);
void         MEM_lockfree_set_memory_debug(void);
size_t       MEM_lockfree_get_memory_in_use(void);
unsigned int MEM_lockfree_get_memory_blocks_in_use(void);
void         MEM_lockfree_reset_peak_memory(void);
size_t       MEM_lockfree_get_peak_memory(void) ATTR_WARN_UNUSED_RESULT;

void         mem_lockfree_clearmemlist(void);

#ifndef NDEBUG
const char* MEM_lockfree_name_ptr(void* vmemh);
void        MEM_lockfree_name_ptr_set(void*       vmemh,
                                      const char* str);
#endif

size_t MEM_guarded_allocN_len(const void* vmemh) ATTR_WARN_UNUSED_RESULT;
#ifdef __cplusplus
void MEM_guarded_freeN(void*                                         vmemh,
                       vektor::mem_guarded::internal::DestructorType destructor_type);
#endif
void* MEM_guarded_dupallocN(const void* vmemh) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT;
void* MEM_guarded_reallocN_id(void*       vmemh,
                              size_t      len,
                              const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(2);
void* MEM_guarded_recallocN_id(void*       vmemh,
                               size_t      len,
                               const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(2);
void* MEM_guarded_callocN(size_t      len,
                          const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(2);
void* MEM_guarded_calloc_arrayN(size_t      len,
                                size_t      size,
                                const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                     2) ATTR_NONNULL(3);
void* MEM_guarded_mallocN(size_t      len,
                          const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(2);
void* MEM_guarded_malloc_arrayN(size_t      len,
                                size_t      size,
                                const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                     2) ATTR_NONNULL(3);
#ifdef __cplusplus
void* MEM_guarded_mallocN_aligned(size_t                                        len,
                                  size_t                                        alignment,
                                  const char*                                   str,
                                  vektor::mem_guarded::internal::DestructorType destructor_type)
    ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1) ATTR_NONNULL(3);
#endif
void* MEM_guarded_malloc_arrayN_aligned(size_t      len,
                                        size_t      size,
                                        size_t      alignment,
                                        const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                             2)
    ATTR_NONNULL(4);
void* MEM_guarded_calloc_arrayN_aligned(size_t      len,
                                        size_t      size,
                                        size_t      alignment,
                                        const char* str) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT ATTR_ALLOC_SIZE(1,
                                                                                                             2)
    ATTR_NONNULL(4);
void         MEM_guarded_printmemlist_pydict(void);
void         MEM_guarded_printmemlist(void);
void         MEM_guarded_callbackmemlist(void (*func)(void*));
void         MEM_guarded_printmemlist_stats(void);
void         MEM_guarded_set_error_callback(void (*func)(const char*));
bool         MEM_guarded_consistency_check(void);
void         MEM_guarded_set_memory_debug(void);
size_t       MEM_guarded_get_memory_in_use(void);
unsigned int MEM_guarded_get_memory_blocks_in_use(void);
void         MEM_guarded_reset_peak_memory(void);
size_t       MEM_guarded_get_peak_memory(void) ATTR_WARN_UNUSED_RESULT;

void         mem_guarded_clearmemlist(void);

#ifndef NDEBUG
const char* MEM_guarded_name_ptr(void* vmemh);
void        MEM_guarded_name_ptr_set(void*       vmemh,
                                     const char* str);
#endif
