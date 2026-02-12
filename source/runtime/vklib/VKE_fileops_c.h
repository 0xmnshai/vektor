#pragma once

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#include <stddef.h>

#include <limits.h>

#include "VKEcompiler_attrs.h"

namespace vektor {
namespace vklib {
FILE *VKE_fopen(const char *filepath, const char *mode) ATTR_WARN_UNUSED_RESULT
    ATTR_NONNULL();
}
}