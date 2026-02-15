#pragma once

#include "wm_keymap.h"
#include "wm_system.h"

namespace vektor
{

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