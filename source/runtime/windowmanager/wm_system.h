#pragma once

#include <list>
#include <map>
#include <string>

#include "wm_event.h"
#include "wm_keymap.h"
#include "wm_types.h"

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

    static wmWindowManager*   get();

    std::shared_ptr<wmWindow> get_window() { return window_; }

    void                      process_events(vkContext* vkC);

    void                      operator_register(const std::string& idname,
                                                wmOperatorType     op);
    wmOperatorType*           operator_find(const std::string& idname);

    void                      on_update(float ts);
    void                      on_render();

    wmKeyConfig*              default_conf;

    void                      push_event(const wmEvent& event);

private:
    std::list<wmEvent>                    event_queue_;
    std::map<std::string, wmOperatorType> operators_;
    std::shared_ptr<wmWindow>             window_;

    void                                  wm_event_do_handlers(vkContext* vkC);
    bool                                  wm_event_do_region_handlers(vkContext*               vkC,
                                                                      std::shared_ptr<ARegion> region,
                                                                      const wmEvent&           event);
    bool                                  wm_event_do_area_handlers(vkContext*               vkC,
                                                                    std::shared_ptr<ScrArea> area,
                                                                    const wmEvent&           event);
    bool                                  wm_event_do_window_handlers(vkContext*     vkC,
                                                                      const wmEvent& event);
};

extern wmWindowManager* G_WM;

void                    init_wm();
} // namespace vektor