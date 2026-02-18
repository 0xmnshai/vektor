#pragma once

#include "GLFW/glfw3.h"
#include "GLFW_ISystem.hh"
#include "GLFW_event_manager.hh" 
#include "GLFW_timer_manager.hh"
#include "GLFW_window_manager.hh"

namespace vektor
{
class GLFW_System : public GLFW_ISystem
{
protected:
    GLFW_System();

    ~GLFW_System() override;

public:
    GLFW_TSuccess              init() override;

    GLFW_TSuccess              exit() override;

    GLFWwindow*                get_window() const { return window_; }

    virtual GLFW_ITimerTask*   install_timer(uint64_t          delay,
                                             uint64_t          interval,
                                             GLFW_TimerProcPtr timer_proc,
                                             GLFW_TUserDataPtr user_data = nullptr);

    virtual GLFW_TSuccess      remove_timer(GLFW_ITimerTask* timerTask);

    virtual GLFW_TSuccess      dispose_window(GLFW_IWindow* window) override;

    void                       dispatch_events() override;

    bool                       valid_window(GLFW_IWindow* window) const override;

    uint64_t                   get_milli_seconds() const override;

    inline GLFW_WindowManager* get_window_manager() const { return window_manager_; }

    inline GLFW_TimerManager*  get_timer_manager() const { return timer_manager_; }

    inline GLFW_EventManager*  get_event_manager() const { return event_manager_; }

    virtual GLFW_TSuccess      add_event_consumer(GLFW_IEventConsumer* consumer) override;

    virtual GLFW_TSuccess      remove_event_consumer(GLFW_IEventConsumer* consumer) override;

    virtual GLFW_TSuccess      push_event(std::unique_ptr<const GLFW_IEvent> event) override;

private:
    bool                native_pixel_;

    GLFW_WindowManager* window_manager_;

    GLFW_TimerManager*  timer_manager_;

    GLFW_EventManager*  event_manager_;

    GLFWwindow*         window_;
};
} // namespace vektor