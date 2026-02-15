#pragma once

namespace vektor
{
enum
{
    SPACE_FLAG_TYPE_TEMPORARY  = (1 << 0),
    SPACE_FLAG_TYPE_WAS_ACTIVE = (1 << 1),
};

#define FILE_MAXDIR 768
#define FILE_MAXFILE 256
#define FILE_MAX 1024

#define FILE_MAX_LIBEXTRA (FILE_MAX + MAX_ID_NAME)

} // namespace vektor