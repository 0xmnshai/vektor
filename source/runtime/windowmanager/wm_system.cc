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
    window_              = std::make_shared<wmWindow>();
    G_WM                 = this;
}

wmWindowManager* wmWindowManager::get()
{
    return G_WM;
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

        if (ISMOUSE_MOTION(event.type))
        {
            CLOG_TRACE(CLG_LogRef_WM, "Mouse Move: type=%d, pos=(%d, %d)", event.type, event.x, event.y);
        }
        else if (ISMOUSE_BUTTON(event.type))
        {
            CLOG_TRACE(CLG_LogRef_WM, "Mouse Button: type=%d, val=%d, pos=(%d, %d)", event.type, event.value, event.x,
                       event.y);
        }
        else if (ISKEYBOARD(event.type))
        {
            CLOG_TRACE(CLG_LogRef_WM, "Keyboard Event: type=%d, val=%d, mods=%d", event.type, event.value,
                       event.modifiers);
        }
        else
        {
            CLOG_TRACE(CLG_LogRef_WM, "Other Event: type=%d, val=%d", event.type, event.value);
        }

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

void wmWindowManager::on_update(float ts)
{
    if (!window_ || !window_->screen)
    {
        return;
    }
    for (auto& area : window_->screen->areabase)
    {
        for (auto& region : area->regionbase)
        {
            if (region->on_update)
            {
                region->on_update(region.get(), ts);
            }
        }
    }
}

void wmWindowManager::on_render()
{
    if (!window_ || !window_->screen)
    {
        return;
    }
    for (auto& area : window_->screen->areabase)
    {
        for (auto& region : area->regionbase)
        {
            if (region->on_draw)
            {
                region->on_draw(region.get());
            }
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