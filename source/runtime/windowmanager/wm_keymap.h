#pragma once

#include <functional>
#include <string>
#include <vector>
#include "wm_event_types.hh"

namespace vektor
{
struct wmEvent;

struct wmOperatorType
{
    std::string                                                              idname;
    std::string                                                              name;
    std::string                                                              description;

    std::function<int(struct vkContext*, struct wmOperator*, const wmEvent*)> exec;
    std::function<bool(struct vkContext*)>                                    poll;
};

struct wmOperator
{
    wmOperatorType* type;
    void*           custom_props;
};

struct wmKeyMapItem
{
    std::string     idname;
    wmEventType     type;
    int16_t         val;
    uint8_t         modifier;
    bool            any_modifier;

    wmOperatorType* op;
};

struct wmKeyMap
{
    std::string               idname;
    std::string               name;
    std::string               description;

    std::vector<wmKeyMapItem> items;

    bool                      poll(struct vkContext* C);
};

struct wmKeyConfig
{
    std::string           idname;
    std::vector<wmKeyMap> keymaps;
};

wmKeyMap*     keymap_add(wmKeyConfig*       config,
                         const std::string& name);

wmKeyMapItem* keymap_add_item(wmKeyMap*          km,
                              const std::string& op_id,
                              wmEventType        type,
                              int                val,
                              int                modifier);

wmKeyMapItem* keymap_find_item(wmKeyMap*      km,
                               const wmEvent* event);
}; // namespace vektor