

#pragma once

#include "DNA_vec_types.h"

namespace vektor
{

namespace ui
{
struct SmoothView2DStore;
}

enum
{
    V2D_LIMITZOOM  = (1 << 0),
    V2D_KEEPASPECT = (1 << 1),
    V2D_KEEPZOOM   = (1 << 2),
    V2D_LOCKZOOM_X = (1 << 8),
    V2D_LOCKZOOM_Y = (1 << 9),
};

enum
{
    V2D_LOCKOFS_X = (1 << 1),
    V2D_LOCKOFS_Y = (1 << 2),
    V2D_KEEPOFS_X = (1 << 3),
    V2D_KEEPOFS_Y = (1 << 4),
};

enum
{
    V2D_KEEPTOT_FREE   = 0,
    V2D_KEEPTOT_BOUNDS = 1,
    V2D_KEEPTOT_STRICT = 2,
};

enum
{
    V2D_VIEWSYNC_SCREEN_TIME   = (1 << 0),
    V2D_VIEWSYNC_AREA_VERTICAL = (1 << 1),
    V2D_PIXELOFS_X             = (1 << 2),
    V2D_PIXELOFS_Y             = (1 << 3),
    V2D_IS_NAVIGATING          = (1 << 9),
    V2D_IS_INIT                = (1 << 10),
    V2D_SNAP_TO_PAGESIZE_Y     = (1 << 11),
    V2D_ZOOM_IGNORE_KEEPOFS    = (1 << 12),
};

enum
{

    V2D_SCROLL_LEFT               = (1 << 0),
    V2D_SCROLL_RIGHT              = (1 << 1),
    V2D_SCROLL_VERTICAL           = (V2D_SCROLL_LEFT | V2D_SCROLL_RIGHT),

    V2D_SCROLL_TOP                = (1 << 2),
    V2D_SCROLL_BOTTOM             = (1 << 3),

    V2D_SCROLL_HORIZONTAL         = (V2D_SCROLL_TOP | V2D_SCROLL_BOTTOM),

    V2D_SCROLL_VERTICAL_HANDLES   = (1 << 5),

    V2D_SCROLL_HORIZONTAL_HANDLES = (1 << 6),

    V2D_SCROLL_VERTICAL_HIDE      = (1 << 7),
    V2D_SCROLL_HORIZONTAL_HIDE    = (1 << 8),

    V2D_SCROLL_VERTICAL_FULLR     = (1 << 9),
    V2D_SCROLL_HORIZONTAL_FULLR   = (1 << 10),
};

enum
{
    V2D_SCROLL_H_ACTIVE = (1 << 0),
    V2D_SCROLL_V_ACTIVE = (1 << 1),
};

enum
{

    V2D_ALIGN_FREE     = 0,

    V2D_ALIGN_NO_POS_X = (1 << 0),
    V2D_ALIGN_NO_NEG_X = (1 << 1),

    V2D_ALIGN_NO_POS_Y = (1 << 2),
    V2D_ALIGN_NO_NEG_Y = (1 << 3),
};

struct View2D
{
    rctf                   tot    = {};
    rctf                   cur    = {};
    rcti                   vert   = {};
    rcti                   hor    = {};
    rcti                   mask   = {};
    float                  min[2] = {}, max[2] = {};
    float                  minzoom = 0, maxzoom = 0;
    short                  scroll    = 0;
    short                  scroll_ui = 0;
    short                  keeptot   = 0;
    short                  keepzoom  = 0;
    short                  keepofs   = 0;
    short                  flag      = 0;
    short                  align     = 0;
    short                  winx = 0, winy = 0;
    short                  oldwinx = 0, oldwiny = 0;
    short                  around     = 0;
    char                   alpha_vert = 0, alpha_hor = 0;
    char                   _pad[2]      = {};
    float                  page_size_y  = 0;
    ui::SmoothView2DStore* sms          = nullptr;
    struct wmTimer*        smooth_timer = nullptr;
};

} // namespace vektor
