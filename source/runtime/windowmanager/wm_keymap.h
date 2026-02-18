#pragma once

#include <string>

#include "wm_event_types.hh"

#include "../dna/DNA_windowmanager_types.h"

namespace vektor
{
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