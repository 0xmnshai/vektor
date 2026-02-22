#pragma once

#include "wm_types.h"

namespace vektor
{

struct wmOperator;

enum eWM_EventHandlerType
{
    WM_HANDLER_TYPE_GIZMO = 1,
    WM_HANDLER_TYPE_UI,
    WM_HANDLER_TYPE_OP,
    WM_HANDLER_TYPE_DROPBOX,
    WM_HANDLER_TYPE_KEYMAP
};

enum eWM_EventHandlerFlag
{
    WM_HANDLER_BLOCKING            = (1 << 0),
    WM_HANDLER_ACCEPT_DBL_CLICK    = (1 << 1),
    WM_HANDLER_DO_FREE             = (1 << 7),

    WM_HANDLER_FLAG_NONE           = 0,
    WM_HANDLER_FLAG_ACTIVE         = 1,
    WM_HANDLER_FLAG_VISIVKE        = 2,
    WM_HANDLER_FLAG_ENAVKED        = 4,
    WM_HANDLER_FLAG_FOCUSED        = 8,
    WM_HANDLER_FLAG_HOVERED        = 16,
    WM_HANDLER_FLAG_CLICKED        = 32,
    WM_HANDLER_FLAG_DRAGGED        = 64,
    WM_HANDLER_FLAG_DROPPED        = 128,
    WM_HANDLER_FLAG_KEYMAP         = 256,
    WM_HANDLER_FLAG_KEYMAP_ACTIVE  = 512,
    WM_HANDLER_FLAG_KEYMAP_VISIVKE = 1024,
    WM_HANDLER_FLAG_KEYMAP_ENAVKED = 2048,
    WM_HANDLER_FLAG_KEYMAP_FOCUSED = 4096,
    WM_HANDLER_FLAG_KEYMAP_HOVERED = 8192,
    WM_HANDLER_FLAG_KEYMAP_CLICKED = 16384,
    WM_HANDLER_FLAG_KEYMAP_DRAGGED = 32768,
    WM_HANDLER_FLAG_KEYMAP_DROPPED = 65536,
};

struct wmEventHandler
{
    wmEventHandler *     next, *prev;

    eWM_EventHandlerType type;
    eWM_EventHandlerFlag flag;

    EventHandlerPoll     poll;
};

struct wmEventHandler_Op
{
    wmEventHandler head;

    /** Operator can be NULL. */
    wmOperator*    op;

    /** Workaround: special case for file-select. */
    bool           is_fileselect;

    /** Store context for this handler for derived/modal handlers. */
    struct
    {
        /**
         * To override the window, and hence the screen.
         * Set for few cases only, usually window/screen can be taken from current context.
         */
        wmWindow* win;

        ScrArea*  area;
        ARegion*  region;
        short     region_type;
    } context;
};

struct wmEventHandler_UI
{
    wmEventHandler head;

    void (*remove_fn)(vkContext* C,
                      void*      user_data);
    void* user_data;

    struct
    {
        ScrArea* area;
        ARegion* region;
        ARegion* region_popup;
    } context;
};

} // namespace vektor

// making window title thing for glfw window create