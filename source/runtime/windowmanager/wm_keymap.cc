#include <iostream>

#include "wm_api.h"
#include "wm_event.h"
#include "wm_event_types.hh"
#include "wm_keymap.h"

#include "../dna/DNA_windowmanager_types.h"

namespace vektor
{
wmKeyMap* keymap_add(wmKeyConfig*       config,
                     const std::string& name)
{
    wmKeyMap* new_km = new wmKeyMap();
    new_km->next     = nullptr;
    new_km->prev     = nullptr;

    // Add to list end
    if (config->keymaps.last)
    {
        ((wmKeyMap*)config->keymaps.last)->next = new_km;
        new_km->prev                            = (wmKeyMap*)config->keymaps.last;
        config->keymaps.last                    = new_km;
    }
    else
    {
        config->keymaps.first = new_km;
        config->keymaps.last  = new_km;
    }

    wmKeyMap* km = new_km;
    km->idname   = name;
    return km;
}

wmKeyMapItem* keymap_add_item(wmKeyMap*          km,
                              const std::string& op_id,
                              wmEventType        type,
                              int                val,
                              int                modifier)
{
    wmKeyMapItem* new_item = new wmKeyMapItem();
    new_item->next         = nullptr;
    new_item->prev         = nullptr;

    // Add to list end
    if (km->items.last)
    {
        ((wmKeyMapItem*)km->items.last)->next = new_item;
        new_item->prev                        = (wmKeyMapItem*)km->items.last;
        km->items.last                        = new_item;
    }
    else
    {
        km->items.first = new_item;
        km->items.last  = new_item;
    }

    wmKeyMapItem* item = new_item;
    item->idname       = op_id;
    item->type         = type;
    item->val          = val;
    item->modifier     = modifier;
    return item;
}

wmKeyMapItem* keymap_find_item(wmKeyMap*      km,
                               const wmEvent* event)
{
    for (wmKeyMapItem* item = (wmKeyMapItem*)km->items.first; item; item = item->next)
    {
        if (item->type == event->type)
        {
            if (item->val == EVENT_NONE || item->val == event->value)
            {
                if (item->any_modifier)
                {
                    return item;
                }

                if (item->modifier == event->modifiers)
                {
                    return item;
                }

                if (item->modifier == 0 && event->modifiers == 0)
                {
                    return item;
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

} // namespace vektor