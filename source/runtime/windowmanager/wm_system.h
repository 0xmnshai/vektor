#pragma once

#include <list>
#include <map>
#include <string>
#include "wm_event.h"
#include "wm_keymap.h"

namespace vektor
{
struct vkContext
{
    void* data;
};

class wmWindowManager
{
public:
    wmWindowManager();
    ~wmWindowManager();

    void            push_event(const wmEvent& event);
    void            process_events(vkContext* C);

    void            operator_register(const std::string& idname,
                                      wmOperatorType     op);
    wmOperatorType* operator_find(const std::string& idname);

    // active key configuration
    wmKeyConfig*    default_conf;

private:
    std::list<wmEvent>                    event_queue;
    std::map<std::string, wmOperatorType> operators;

    void                                  wm_event_do_handlers(vkContext* C);
};

extern wmWindowManager* G_WM;

void                    init_wm();
} // namespace vektor