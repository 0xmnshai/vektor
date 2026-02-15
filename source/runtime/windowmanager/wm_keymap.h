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
    std::string                                                               idname;
    std::string                                                               name;
    std::string                                                               description;

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

    bool                      poll(struct vkContext* vkC);
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

typedef int (*operator_exec_callback)(struct bContext*      C,
                                      struct wmOperator*    op,
                                      const struct wmEvent* event);
typedef int (*operator_invoke_callback)(struct bContext*      C,
                                        struct wmOperator*    op,
                                        const struct wmEvent* event);
typedef bool (*operator_poll_callback)(struct bContext* C);

void         API_keymap_item_set_flag(wmKeyMapItem* kmi,
                                      bool          any_modifier);

void         operator_register(const char*              idname,
                               const char*              name,
                               const char*              description,
                               operator_exec_callback   exec,
                               operator_invoke_callback invoke = nullptr,
                               operator_poll_callback   poll   = nullptr);

wmKeyConfig* get_default_keyconfig();

void         event_push(int type,
                        int val,
                        int modifiers,
                        int x = 0,
                        int y = 0);
}; // namespace vektor