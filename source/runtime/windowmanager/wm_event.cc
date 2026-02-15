#include <chrono>
#include <iostream>
#include <thread>

#include "wm_event.h"
#include "wm_event_types.hh"
#include "wm_keymap.h"
#include "wm_system.h"

namespace vektor
{

int MESH_OT_select_all_exec(vkContext*     vkC,
                            wmOperator*    op,
                            const wmEvent* event)
{
    std::cout << "\n>>> [OPERATOR] MESH_OT_select_all Executed! <<<\n";
    std::cout << "    Context: " << (vkC ? "Valid" : "Null") << "\n";
    std::cout << "    Triggered by: " << (event->is_keyboard() ? "Keyboard" : "Mouse") << "\n";
    return 1;
}

int SCREEN_OT_action_zone_exec(vkContext*     vkC,
                               wmOperator*    op,
                               const wmEvent* event)
{
    std::cout << "\n>>> [OPERATOR] SCREEN_OT_action_zone Executed! <<<\n";
    std::cout << "    Context: " << (vkC ? "Valid" : "Null") << "\n";
    std::cout << "    Triggered by: " << (event->is_keyboard() ? "Keyboard" : "Mouse") << "\n";
    return 1;
}

void setup_operators()
{
    wmOperatorType op;
    op.name        = "Select All";
    op.description = "Toggle selection of all items";
    op.exec        = MESH_OT_select_all_exec;
    G_WM->operator_register("MESH_OT_select_all", op);

    wmOperatorType op2;
    op2.name = "Action Zone";
    op2.exec = SCREEN_OT_action_zone_exec;
    G_WM->operator_register("SCREEN_OT_action_zone", op2);
}

void setup_keymap()
{
    wmKeyConfig* conf = get_default_keyconfig();
    if (!conf)
        return;

    wmKeyMap* km = keymap_add(conf, "Window");

    keymap_add_item(km, "MESH_OT_select_all", EVT_AKEY, EVT_PRESS, 0); // No modifiers

    keymap_add_item(km, "SCREEN_OT_action_zone", EVT_AKEY, EVT_PRESS, EVT_CTRL);
}

void simulate_vektor_input()
{
    std::cout << "[VEKTOR] Simulating OS Inputs...\n";

    std::cout << "[VEKTOR] User pressed 'A'\n";
    event_push(EVT_AKEY, EVT_PRESS, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "[VEKTOR] User pressed 'Ctrl + A'\n";
    event_push(EVT_AKEY, EVT_PRESS, EVT_CTRL);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "[VEKTOR] User Clicked Left Mouse\n";
    event_push(LEFTMOUSE, EVT_PRESS, 0, 100, 200);
}
} // namespace vektor