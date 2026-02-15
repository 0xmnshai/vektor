
#include <iostream>

#include "wm_event.h"
#include "wm_event_types.hh"
#include "wm_keymap.h"
#include "wm_system.h"

namespace vektor
{
wmKeyMap* keymap_add(wmKeyConfig*       config,
                     const std::string& name)
{
    config->keymaps.emplace_back(wmKeyMap());
    wmKeyMap* km = &config->keymaps.back();
    km->idname   = name;
    return km;
}

wmKeyMapItem* keymap_add_item(wmKeyMap*          km,
                              const std::string& op_id,
                              wmEventType        type,
                              int                val,
                              int                modifier)
{
    km->items.emplace_back(wmKeyMapItem());
    wmKeyMapItem* item = &km->items.back();
    item->idname       = op_id;
    item->type         = type;
    item->val          = val;
    item->modifier     = modifier;
    return item;
}

wmKeyMapItem* keymap_find_item(wmKeyMap*      km,
                               const wmEvent* event)
{
    for (wmKeyMapItem& item : km->items)
    {
        if (item.type == event->type)
        {
            if (item.val == EVENT_NONE || item.val == event->value)
            {
                if (item.any_modifier)
                {
                    return &item;
                }

                if (item.modifier == event->modifiers)
                {
                    return &item;
                }

                if (item.modifier == 0 && event->modifiers == 0)
                {
                    return &item;
                }
            }
        }
    }
    return nullptr;
}

void API_keymap_item_set_flag(wmKeyMapItem* kmi,
                              bool          any_modifier)
{
    if (kmi)
    {
        kmi->any_modifier = any_modifier;
    }
}

void API_operator_register(const char*              idname,
                           const char*              name,
                           const char*              description,
                           operator_exec_callback   exec,
                           operator_invoke_callback invoke,
                           operator_poll_callback   poll)
{
    if (!G_WM)
    {
        std::cerr << "API Error: WM not initialized. Call API_init_wm() first.\n";
        return;
    }

    wmOperatorType op;

    if (idname)
        op.idname = idname;
    if (name)
        op.name = name;
    if (description)
        op.description = description;

    G_WM->operator_register(idname ? idname : "UNKNOWN", op);
}

void event_push(int type,
                int val,
                int modifiers,
                int x,
                int y)
{
    if (!G_WM)
    {
        return;
    }

    wmEvent evt;
    evt.type      = (wmEventType)type;
    evt.value     = val;
    evt.modifiers = modifiers;
    evt.x         = x;
    evt.y         = y;

    G_WM->push_event(evt);
}

wmKeyConfig* get_default_keyconfig()
{
    if (!G_WM)
    {
        return nullptr;
    }
    return G_WM->default_conf;
}

bool wmKeyMap::poll(struct vkContext* vkC)
{
    return true; // Always active for this demo
}

} // namespace vektor