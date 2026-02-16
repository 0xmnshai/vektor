#pragma once

#include <cstring>
#include "../../intern/gaurdalloc/mallocn_defines.hh"

namespace vektor
{
constexpr char SEP = '/';
#define SEP_STR "/"

#define VKE_path_ncmp strncmp

const char* VKE_path_extension_or_end(const char* filepath) ATTR_NONNULL(1) ATTR_WARN_UNUSED_RESULT;

const char* VKE_path_slash_rfind(const char* path) ATTR_NONNULL(1) ATTR_WARN_UNUSED_RESULT;

const char* VKE_path_basename(const char* path);
} // namespace vektor