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

enum wmTimerFlags
{
    WM_TIMER_NO_FREE_CUSTOM_DATA = 1 << 0,
    WM_TIMER_TAGGED_FOR_REMOVAL  = 1 << 16,
};

struct wmTimer
{
    wmTimer *    next, *prev;

    /** Window this timer is attached to (optional). */
    wmWindow*    win;

    /** Set by timer user. */
    double       time_step;

    /** Set by timer user, goes to event system. */
    wmEventType  event_type;

    /** Various flags controlling timer options, see below. */
    wmTimerFlags flags;

    /** Set by timer user, to allow custom values. */
    void*        customdata;

    /** Total running time in seconds. */
    double       time_duration;
    /** Time since previous step in seconds. */
    double       time_delta;

    /** Internal, last time timer was activated. */
    double       time_last;
    /** Internal, next time we want to activate the timer. */
    double       time_next;
    /** Internal, when the timer started. */
    double       time_start;
    /** Internal, put timers to sleep when needed. */
    bool         sleep;
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

    short                     file_saved = 0;

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