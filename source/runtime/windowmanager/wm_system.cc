#include "wm_system.h"
#include <iostream>
#include "wm_keymap.h"

namespace vektor
{

wmWindowManager* G_WM = nullptr;

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
    event_queue.push_back(event);
}

void wmWindowManager::process_events(vkContext* C)
{
    while (!event_queue.empty())
    {
        const wmEvent& event = event_queue.front();

        wm_event_do_handlers(C);

        event_queue.pop_front();
    }
}

void wmWindowManager::operator_register(const std::string& idname,
                                        wmOperatorType     op)
{
    op.idname         = idname;
    operators[idname] = op;
}

wmOperatorType* wmWindowManager::operator_find(const std::string& idname)
{
    auto it = operators.find(idname);
    if (it != operators.end())
    {
        return &it->second;
    }
    return nullptr;
};

void wmWindowManager::wm_event_do_handlers(vkContext* C)
{
    if (event_queue.empty())
        return;

    const wmEvent* event = &event_queue.front();

    for (auto& km : default_conf->keymaps)
    {
        if (!km.poll(C))
            continue;

        wmKeyMapItem* kmi = keymap_find_item(&km, event);
        if (kmi)
        {
            std::cout << "[Event System] Matched Keymap: " << kmi->idname << std::endl;

            wmOperatorType* op_type = operator_find(kmi->idname);
            if (op_type)
            {
                if (!op_type->poll || op_type->poll(C))
                {
                    wmOperator op_instance;
                    op_instance.type = op_type;
                    op_type->exec(C, &op_instance, event);
                }
            }
            return;
        }
    }
}
} // namespace vektor