#pragma once

#include <cstdint>

#include "wm_event_types.hh"
#include "wm_keymap.h"

namespace vektor
{

    
struct wmEvent
{
    struct wmEvent *next = nullptr, *prev = nullptr;

    wmEventType     type  = EVENT_NONE;
    int16_t         value = 0;

    int             x = 0, y = 0;
    int             prev_x = 0, prev_y = 0;

    uint8_t         modifiers  = 0;

    void*           customdata = nullptr;

    double          time       = 0.0;

    bool            is_keyboard() const
    {
        return (type >= EVT_AKEY && type <= EVT_ZKEY) || (type >= EVT_ZEROKEY && type <= EVT_TWOKEY) ||
               (type >= EVT_ESCKEY && type <= EVT_LEFTALTKEY);
    }
    bool is_mouse_move() const { return type == MOUSEMOVE; }
};

int  MESH_OT_select_all_exec(vkContext*     vkC,
                             wmOperator*    op,
                             const wmEvent* event);

int  SCREEN_OT_action_zone_exec(vkContext*     vkC,
                                wmOperator*    op,
                                const wmEvent* event);

void setup_operators();
void setup_keymap();
void simulate_vektor_input();
} // namespace vektor
