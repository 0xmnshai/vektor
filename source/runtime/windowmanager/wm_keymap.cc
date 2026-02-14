
#include "wm_keymap.h"
#include "wm_event.h"
#include "wm_event_types.hh"

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

bool wmKeyMap::poll(struct vkContext* C)
{
    return true; // Always active for this demo
}

} // namespace vektor