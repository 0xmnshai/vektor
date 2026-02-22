#include "wm_cursors.hh"

#include <algorithm>
#include <cmath>
#include <string>

#define NANOSVG_ALL_COLOR_KEYWORDS // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION     // Expands implementation
#include <nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

#include "../clog/COG_log.hh"
#include "../dna/DNA_windowmanager_types.h"
#include "../kernel/VK_global.hh"
#include "GLFW_Window.hh"
#include "GLFW_types.h"

#include "../vklib/VKE_utildefines.h"
#include "wm_api.h"

using namespace vektor::vklib;

namespace vektor
{

constexpr int CURSOR_HARDWARE_SIZE_MAX = 255;

CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_App,
                          "WindowManager");

static GLFW_TStandardCursor convert_to_glfw_standard_cursor(WMCursorType curs)
{
    switch (curs)
    {
        case WM_CURSOR_DEFAULT:
            return GLFW_kStandardCursorDefault;
        case WM_CURSOR_WAIT:
            return GLFW_kStandardCursorWait;
        case WM_CURSOR_EDIT:
        case WM_CURSOR_CROSS:
            return GLFW_kStandardCursorCrosshair;
        case WM_CURSOR_MOVE:
            return GLFW_kStandardCursorMove;
        case WM_CURSOR_X_MOVE:
            return GLFW_kStandardCursorLeftRight;
        case WM_CURSOR_Y_MOVE:
            return GLFW_kStandardCursorUpDown;
        case WM_CURSOR_COPY:
            return GLFW_kStandardCursorCopy;
        case WM_CURSOR_HAND:
            return GLFW_kStandardCursorHandOpen;
        case WM_CURSOR_HAND_CLOSED:
            return GLFW_kStandardCursorHandClosed;
        case WM_CURSOR_HAND_POINT:
            return GLFW_kStandardCursorHandPoint;
        case WM_CURSOR_H_SPLIT:
            return GLFW_kStandardCursorHorizontalSplit;
        case WM_CURSOR_V_SPLIT:
            return GLFW_kStandardCursorVerticalSplit;
        case WM_CURSOR_STOP:
            return GLFW_kStandardCursorStop;
        case WM_CURSOR_KNIFE:
            return GLFW_kStandardCursorKnife;
        case WM_CURSOR_NSEW_SCROLL:
            return GLFW_kStandardCursorNSEWScroll;
        case WM_CURSOR_NS_SCROLL:
            return GLFW_kStandardCursorNSScroll;
        case WM_CURSOR_EW_SCROLL:
            return GLFW_kStandardCursorEWScroll;
        case WM_CURSOR_EYEDROPPER:
            return GLFW_kStandardCursorEyedropper;
        case WM_CURSOR_N_ARROW:
            return GLFW_kStandardCursorUpArrow;
        case WM_CURSOR_S_ARROW:
            return GLFW_kStandardCursorDownArrow;
        case WM_CURSOR_PAINT:
            return GLFW_kStandardCursorCrosshairA;
        case WM_CURSOR_DOT:
            return GLFW_kStandardCursorCrosshairB;
        case WM_CURSOR_CROSSC:
            return GLFW_kStandardCursorCrosshairC;
        case WM_CURSOR_ERASER:
            return GLFW_kStandardCursorEraser;
        case WM_CURSOR_ZOOM_IN:
            return GLFW_kStandardCursorZoomIn;
        case WM_CURSOR_ZOOM_OUT:
            return GLFW_kStandardCursorZoomOut;
        case WM_CURSOR_TEXT_EDIT:
            return GLFW_kStandardCursorText;
        case WM_CURSOR_PAINT_BRUSH:
            return GLFW_kStandardCursorPencil;
        case WM_CURSOR_E_ARROW:
            return GLFW_kStandardCursorRightArrow;
        case WM_CURSOR_W_ARROW:
            return GLFW_kStandardCursorLeftArrow;
        case WM_CURSOR_LEFT_HANDLE:
            return GLFW_kStandardCursorLeftHandle;
        case WM_CURSOR_RIGHT_HANDLE:
            return GLFW_kStandardCursorRightHandle;
        case WM_CURSOR_BOTH_HANDLES:
            return GLFW_kStandardCursorBothHandles;
        case WM_CURSOR_BLADE:
            return GLFW_kStandardCursorBlade;
        case WM_CURSOR_SLIP:
            return GLFW_kStandardCursorSlip;
        default:
            return GLFW_kStandardCursorCustom;
    }
}

struct BCursorHotspot
{
    int x;
    int y;
};

struct BCursor
{
    /**
     * An SVG document size of 1600x1600 being the "normal" size,
     * cropped to the image size and without any padding.
     */
    const char*    svg_source;
    /**
     * A factor (0-1) from the top-left corner of the image (not of the document size).
     */
    BCursorHotspot hotspot;
    /**
     * By default cursors are "light", allow dark themes to invert.
     */
    bool           can_invert;
};

static BCursor  g_cursors[WM_CURSOR_NUM] = {{nullptr}};

static uint8_t* cursor_bitmap_from_svg(const char* svg,
                                       const int   cursor_size,
                                       uint8_t* (*alloc_fn)(size_t size),
                                       int r_bitmap_size[2])
{
    std::string svg_source = svg;

    NSVGimage*  image      = nsvgParse(svg_source.data(), "px", 96.0f);
    if (image == nullptr)
    {
        return nullptr;
    }
    if (image->width == 0 || image->height == 0)
    {
        nsvgDelete(image);
        return nullptr;
    }
    NSVGrasterizer* rast = nsvgCreateRasterizer();
    if (rast == nullptr)
    {
        nsvgDelete(image);
        return nullptr;
    }

    const float  scale        = float(cursor_size) / 1600.0f;
    const size_t dest_size[2] = {
        std::min(size_t(ceil(image->width * scale)), size_t(cursor_size)),
        std::min(size_t(ceil(image->height * scale)), size_t(cursor_size)),
    };

    uint8_t* bitmap_rgba = alloc_fn(sizeof(uint8_t[4]) * dest_size[0] * dest_size[1]);
    if (bitmap_rgba == nullptr)
    {
        return nullptr;
    }

    nsvgRasterize(rast, image, 0.0f, 0.0f, scale, bitmap_rgba, dest_size[0], dest_size[1], dest_size[0] * 4);

    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    r_bitmap_size[0] = dest_size[0];
    r_bitmap_size[1] = dest_size[1];

    return bitmap_rgba;
}

static bool window_set_custom_cursor_generator(wmWindow*      win,
                                               const BCursor& cursor)
{
    GLFW_CursorGenerator* cursor_generator = MEM_new_zeroed<GLFW_CursorGenerator>(__func__);
    cursor_generator->generate_fn          = [](const GLFW_CursorGenerator* cursor_generator, const int cursor_size,
                                       const int cursor_size_max, uint8_t* (*alloc_fn)(size_t size),
                                       int r_bitmap_size[2], int r_hot_spot[2], bool* r_can_invert_color) -> uint8_t*
    {
        const BCursor& cursor = *static_cast<const BCursor*>(cursor_generator->user_data);
        /* Currently SVG uses the `cursor_size` as the maximum. */
        UNUSED_VARS(cursor_size_max);

        int      bitmap_size[2];
        uint8_t* bitmap_rgba = cursor_bitmap_from_svg(cursor.svg_source, cursor_size, alloc_fn, bitmap_size);

        if (UNLIKELY(bitmap_rgba == nullptr))
        {
            return nullptr;
        }

        r_bitmap_size[0]    = bitmap_size[0];
        r_bitmap_size[1]    = bitmap_size[1];

        r_hot_spot[0]       = int(cursor.hotspot.x * (bitmap_size[0] - 1));
        r_hot_spot[1]       = int(cursor.hotspot.y * (bitmap_size[1] - 1));

        *r_can_invert_color = cursor.can_invert;

        return bitmap_rgba;
    };

    cursor_generator->user_data = const_cast<void*>(static_cast<const void*>(&cursor));
    cursor_generator->free_fn   = [](GLFW_CursorGenerator* cursor_generator) { MEM_delete(cursor_generator); };

    GLFW_Window*  glfw_window   = static_cast<GLFW_Window*>(win->runtime->glfw_win);
    GLFW_TSuccess success       = glfw_window->set_custom_cursor_generator(cursor_generator);

    return (success == GLFW_kSuccess) ? true : false;
}

static int wm_cursor_size(const wmWindow* win)
{
    // /* The DPI as a scale without the UI scale preference. */
    // const float system_scale = WM_window_dpi_get_scale(win);
    // return std::lround(WM_cursor_preferred_logical_size() * system_scale);

    return 21;
}

static void cursor_rgba_to_xbm_32(const uint8_t* rgba,
                                  const int      bitmap_size[2],
                                  uint8_t*       bitmap,
                                  uint8_t*       mask)
{
    for (int y = 0; y < bitmap_size[1]; y++)
    {
        for (int x = 0; x < bitmap_size[0]; x++)
        {
            int i = (y * bitmap_size[0] * 4) + (x * 4);
            int j = (y * 4) + (x >> 3);
            int k = (x % 8);
            if (rgba[i + 3] > 128)
            {
                if (rgba[i] > 128)
                {
                    bitmap[j] |= (1 << k);
                }
                mask[j] |= (1 << k);
            }
        }
    }
}

static bool window_set_custom_cursor_pixmap(wmWindow*      win,
                                            const BCursor& cursor)
{
    /* Option to force use of 1bpp XBitMap cursors is needed for testing. */
    const bool use_only_1bpp_cursors = false;

    const bool use_rgba              = !use_only_1bpp_cursors && (WM_capabilities_flag() & WM_CAPABILITY_CURSOR_RGBA);

    const int  max_size              = use_rgba ? CURSOR_HARDWARE_SIZE_MAX : 32;
    const int  size                  = std::min(wm_cursor_size(win), max_size);

    int        bitmap_size[2]        = {0, 0};

    uint8_t*   bitmap_rgba           = cursor_bitmap_from_svg(
        cursor.svg_source, size,
        [](size_t size) -> uint8_t* { return MEM_new_array_uninitialized<uint8_t>(size, "wm.cursor"); }, bitmap_size);

    if (UNLIKELY(bitmap_rgba == nullptr))
    {
        return false;
    }

    const int hot_spot[2] = {
        int(cursor.hotspot.x * (bitmap_size[0] - 1)),
        int(cursor.hotspot.y * (bitmap_size[1] - 1)),
    };

    GLFW_TSuccess success;

    GLFW_Window*  glfw_window = static_cast<GLFW_Window*>(win->runtime->glfw_win);
    if (use_rgba)
    {
        success = glfw_window->set_custom_cursor_shape(bitmap_rgba, nullptr, bitmap_size, hot_spot, cursor.can_invert);
    }
    else
    {
        int     bitmap_size_fixed[2] = {32, 32};

        uint8_t bitmap[4 * 32]       = {0};
        uint8_t mask[4 * 32]         = {0};
        cursor_rgba_to_xbm_32(bitmap_rgba, bitmap_size, bitmap, mask);
        success = glfw_window->set_custom_cursor_shape(bitmap, mask, bitmap_size_fixed, hot_spot, cursor.can_invert);
    }

    MEM_delete(bitmap_rgba);
    return (success == GLFW_kSuccess) ? true : false;
}

static bool window_set_custom_cursor(wmWindow*      win,
                                     const BCursor& cursor)
{
    if (WM_capabilities_flag() & WM_CAPABILITY_CURSOR_GENERATOR)
    {
        return window_set_custom_cursor_generator(win, cursor);
    }
    return window_set_custom_cursor_pixmap(win, cursor);
}

void WM_cursor_set(wmWindow* win,
                   int       curs)
{
    /* Option to not use any OS-supplied cursors is needed for testing. */
    const bool use_only_custom_cursors = false;

    if (G.background)
    {
        return;
    }

    if (win == nullptr)
    {
        /* Can't set custom cursor before window initialization. */
        return;
    }

    if (curs == WM_CURSOR_DEFAULT && win->modalcursor)
    {
        /* If the cursor was set to default during the modal operation,
         * this usually indicates that win->lastcursor is not relevant anymore.
         * So update lastcursor to the default cursor as this is usually a safe
         * cursor shape to fall back to (see #144345).
         */
        win->lastcursor = curs;
        curs            = win->modalcursor;
    }

    GLFW_Window* glfw_window = static_cast<GLFW_Window*>(win->runtime->glfw_win);

    if (curs == WM_CURSOR_NONE)
    {
        glfw_window->set_cursor_visibility(false);
        return;
    }

    glfw_window->set_cursor_visibility(true);

    if (win->cursor == curs)
    {
        return; /* Cursor is already set. */
    }

    win->cursor = curs;

    if (curs < 0 || curs >= WM_CURSOR_NUM)
    {
        CLOG_ERROR(CLG_LogRef_App, "Invalid cursor number");
        return;
    }

    GLFW_TStandardCursor glfw_cursor = convert_to_glfw_standard_cursor(WMCursorType(curs));

    if (!use_only_custom_cursors && glfw_cursor != GLFW_kStandardCursorCustom &&
        glfw_window->has_cursor_shape(glfw_cursor))
    {
        /* Use native GHOST cursor when available. */
        glfw_window->get_standard_cursor_shape(glfw_cursor);
    }
    else
    {
        const BCursor& bcursor = g_cursors[curs];
        if (!bcursor.svg_source || !window_set_custom_cursor(win, bcursor))
        {
            /* Fall back to default cursor if no bitmap found. */
            glfw_window->set_cursor_shape(GLFW_kStandardCursorDefault);
        }
    }
}

void WM_cursor_modal_set(wmWindow* win,
                         int       val)
{
    if (win->lastcursor == 0)
    {
        win->lastcursor = win->cursor;
    }
    win->modalcursor = val;
    WM_cursor_set(win, val);
}

void WM_cursor_modal_restore(wmWindow* win)
{
    if (win->modalcursor != 0)
    {
        win->modalcursor = 0;
        WM_cursor_set(win, win->lastcursor);
    }
}

void WM_cursor_wait(bool val)
{
    if (!G.background)
    {
        wmWindowManager* wm  = static_cast<wmWindowManager*>(G_MAIN->wm.first);
        wmWindow*        win = static_cast<wmWindow*>(wm ? wm->windows.first : nullptr);

        for (; win; win = win->next)
        {
            if (val)
            {
                WM_cursor_modal_set(win, WM_CURSOR_WAIT);
            }
            else
            {
                WM_cursor_modal_restore(win);
            }
        }
    }
}

} // namespace vektor
