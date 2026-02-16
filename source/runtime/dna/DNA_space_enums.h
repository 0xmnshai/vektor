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

enum eSpace_Type
{
    SPACE_EMPTY       = 0,
    SPACE_VIEW3D      = 1,
    SPACE_GRAPH       = 2,
    SPACE_OUTLINER    = 3,
    SPACE_PROPERTIES  = 4,
    SPACE_FILE        = 5,
    SPACE_IMAGE       = 6,
    SPACE_INFO        = 7,
    SPACE_SEQ         = 8,
    SPACE_TEXT        = 9,

    SPACE_CONSOLE     = 10,
    SPACE_USERPREF    = 12,
    SPACE_CLIP        = 12,
    SPACE_TOPBAR      = 13,
    SPACE_STATUSBAR   = 14,
    SPACE_SPREADSHEET = 15
};

#define SPACE_TYPE_ANY -1

#define IMG_SIZE_FALLBACK 256
} // namespace vektor