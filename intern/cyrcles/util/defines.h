#define ccl_device_inline static inline __attribute__((always_inline))


#define ccl_global
#define ccl_inline_constant inline constexpr
#define ccl_static_constexpr static constexpr
#define ccl_constant const
#define ccl_private
#define ccl_ray_data ccl_private

#define ccl_restrict __restrict
#define ccl_optional_struct_init
#define ccl_attr_maybe_unused [[maybe_unused]]
#define __KERNEL_WITH_SSE_ALIGN__

#ifndef __KERNEL_GPU__
#include <cassert>
#define util_assert(statement) assert(statement)
#else
#define util_assert(statement)
#endif