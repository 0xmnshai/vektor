#pragma once

#include <cstdint>
#include "wm_event_types.hh"

namespace vektor
{
struct wmEvent
{
    struct wmEvent *  next, *prev;

    const wmEventType type;
    const int16_t     value;

    const int         x, y;
    const int         prev_x, prev_y;

    const uint8_t     modifiers;

    void*             customdata;

    const double      time;

    bool              is_keyboard() const
    {
        return (type >= EVT_AKEY && type <= EVT_ZKEY) || (type >= EVT_ZEROKEY && type <= EVT_TWOKEY) ||
               (type >= EVT_ESCKEY && type <= EVT_LEFTALTKEY);
    }

    bool is_mouse_move() const { return type == MOUSEMOVE; }
};
} // namespace vektor
