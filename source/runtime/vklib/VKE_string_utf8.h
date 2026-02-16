#pragma once

#include <cstddef>

#include "../../intern/gaurdalloc/mallocn_defines.hh"

namespace vektor
{

char*                     VKE_strncpy_utf8(char* __restrict dst,
                                           const char* __restrict src,
                                           size_t dst_maxncpy) ATTR_NONNULL(1,
                                                                            2);

size_t                    VKE_strncpy_utf8_rlen(char* __restrict dst,
                                                const char* __restrict src,
                                                size_t dst_maxncpy) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL(1,
                                                                                                         2);

[[nodiscard]] const char* VKE_str_utf8_invalid_substitute_if_needed(const char*  str,
                                                                    size_t       str_len,
                                                                    const char   substitute,
                                                                    char*        buf,
                                                                    const size_t buf_maxncpy) ATTR_NONNULL(1,
                                                                                                           4);
} // namespace vektor