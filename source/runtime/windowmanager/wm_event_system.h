#pragma once

#include "wm_types.h"

namespace vektor
{
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
    WM_HANDLER_FLAG_VISIBLE        = 2,
    WM_HANDLER_FLAG_ENABLED        = 4,
    WM_HANDLER_FLAG_FOCUSED        = 8,
    WM_HANDLER_FLAG_HOVERED        = 16,
    WM_HANDLER_FLAG_CLICKED        = 32,
    WM_HANDLER_FLAG_DRAGGED        = 64,
    WM_HANDLER_FLAG_DROPPED        = 128,
    WM_HANDLER_FLAG_KEYMAP         = 256,
    WM_HANDLER_FLAG_KEYMAP_ACTIVE  = 512,
    WM_HANDLER_FLAG_KEYMAP_VISIBLE = 1024,
    WM_HANDLER_FLAG_KEYMAP_ENABLED = 2048,
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

} // namespace vektor

// making window title thing for glfw window create