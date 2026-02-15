#include "wm_system.h"
#include <iostream>
#include "wm_keymap.h"

#include "../../../intern/clog/COG_log.hh"

namespace vektor
{
wmWindowManager* G_WM = nullptr;
CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_WM,
                          "WM");

wmWindowManager::wmWindowManager()
{
    default_conf         = new wmKeyConfig();
    default_conf->idname = "Default";
    G_WM                 = this;
}

wmWindowManager::~wmWindowManager()
{
    delete default_conf;
    G_WM = nullptr;
}

void wmWindowManager::push_event(const wmEvent& event)
{
    event_queue_.push_back(event);
}

void wmWindowManager::process_events(vkContext* vkC)
{
    while (!event_queue_.empty())
    {
        const wmEvent& event = event_queue_.front();

        wm_event_do_handlers(vkC);

        event_queue_.pop_front();
    }
}

void wmWindowManager::operator_register(const std::string& idname,
                                        wmOperatorType     op)
{
    op.idname          = idname;
    operators_[idname] = op;
}

wmOperatorType* wmWindowManager::operator_find(const std::string& idname)
{
    auto it = operators_.find(idname);
    if (it != operators_.end())
    {
        return &it->second;
    }
    return nullptr;
};

void wmWindowManager::wm_event_do_handlers(vkContext* vkC)
{
    if (event_queue_.empty())
        return;

    const wmEvent* event = &event_queue_.front();

    for (auto& km : default_conf->keymaps)
    {
        if (!km.poll(vkC))
            continue;

        wmKeyMapItem* kmi = keymap_find_item(&km, event);
        if (kmi)
        {
            std::cout << "[Event System] Matched Keymap: " << kmi->idname << std::endl;

            CLOG_TRACE(CLG_LogRef_WM, "Matched Keymap: %s", kmi->idname.c_str());

            wmOperatorType* op_type = operator_find(kmi->idname);
            if (op_type)
            {
                if (!op_type->poll || op_type->poll(vkC))
                {
                    wmOperator op_instance;
                    op_instance.type = op_type;
                    op_type->exec(vkC, &op_instance, event);
                }
            }
            return;
        }
    }
}

void init_wm()
{
    if (!G_WM)
    {
        CLG_logref_init(CLG_LogRef_WM);
        static wmWindowManager* wm_instance = new wmWindowManager();
    }
}
} // namespace vektor