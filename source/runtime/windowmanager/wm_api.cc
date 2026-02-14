#include <iostream>

#include "wm_event.h"
#include "wm_event_types.hh"
#include "wm_keymap.h"
#include "wm_system.h"

// Mock Context
struct bContext
{
    void* data;
};

namespace vektor
{
extern wmWindowManager* G_WM;

int                     MESH_OT_select_all_exec(vkContext*     C,
                                                wmOperator*    op,
                                                const wmEvent* event)
{
    std::cout << "\n>>> [OPERATOR] MESH_OT_select_all Executed! <<<\n";
    std::cout << "    Context: " << (C ? "Valid" : "Null") << "\n";
    std::cout << "    Triggered by: " << (event->is_keyboard() ? "Keyboard" : "Mouse") << "\n";
    return 1;
}

int SCREEN_OT_action_zone_exec(vkContext*     C,
                               wmOperator*    op,
                               const wmEvent* event)
{
    std::cout << "\n>>> [OPERATOR] SCREEN_OT_action_zone Executed! <<<\n";
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
    // Mimic Blender's default keymap setup
    wmKeyMap* wm_keymap = keymap_add(G_WM->default_conf, "Window");

    // Bind 'A' to Select All
    keymap_add_item(wm_keymap, "MESH_OT_select_all", EVT_AKEY, EVT_PRESS, 0); // No modifiers

    // Bind 'Ctrl+A' to Action Zone (just for demo)
    keymap_add_item(wm_keymap, "SCREEN_OT_action_zone", EVT_AKEY, EVT_PRESS, EVT_CTRL);
}

// void simulate_input()
// {
//   std::cout << "] Simulating OS Inputs...\n";

//   // 1. User presses 'A'
//   std::cout << "] User pressed 'A'\n";
//   wmEvent evt_a;
//   evt_a.type = EVT_AKEY;
//   evt_a.val = KM_PRESS;
//   evt_a.modifiers = 0;
//   G_WM->push_event(evt_a);

//   std::this_thread::sleep_for(std::chrono::milliseconds(500));

//   // 2. User presses 'Ctrl + A'
//   std::cout << "] User pressed 'Ctrl + A'\n";
//   wmEvent evt_ctrl_a;
//   evt_ctrl_a.type = EVT_AKEY;
//   evt_ctrl_a.val = KM_PRESS;
//   evt_ctrl_a.modifiers = KM_CTRL;
//   G_WM->push_event(evt_ctrl_a);

//   std::this_thread::sleep_for(std::chrono::milliseconds(500));

//   // 3. User clicks mouse (no binding, should be ignored by keymap but printed by debug)
//   std::cout << "] User Clicked Left Mouse\n";
//   wmEvent evt_mouse;
//   evt_mouse.type = LEFTMOUSE;
//   evt_mouse.val = KM_PRESS;
//   evt_mouse.modifiers = 0;
//   evt_mouse.x = 100;
//   evt_mouse.y = 200;
//   G_WM->push_event(evt_mouse);
// }

} // namespace vektor