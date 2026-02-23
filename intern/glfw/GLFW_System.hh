#pragma once

#include <memory>

#include "GLFW_ISystem.hh"
#include "GLFW_event_manager.hh"
#include "GLFW_timer_manager.hh"
#include "GLFW_window_manager.hh"

namespace vektor
{
class GLFW_System : public GLFW_ISystem
{
public:
    GLFW_System();

    ~GLFW_System() override;
    GLFW_TSuccess              init() override;

    GLFW_TSuccess              exit() override;

    virtual GLFW_TSuccess      create_system(bool verbose    = true,
                                             bool background = false) override;

    virtual GLFW_TSuccess      create_system_background() override;

    inline GLFW_ISystem*       get_system() const override { return system_; };

    GLFW_TSuccess              destroy_system() const override;

    virtual GLFW_ITimerTask*   install_timer(uint64_t          delay,
                                             uint64_t          interval,
                                             GLFW_TimerProcPtr timer_proc,
                                             GLFW_TUserDataPtr user_data = nullptr);

    virtual GLFW_TSuccess      remove_timer(GLFW_ITimerTask* timerTask);

    virtual GLFW_TSuccess      dispose_window(GLFW_IWindow* window) override;

    void                       dispatch_events() override;

    bool                       valid_window(GLFW_IWindow* window) const override;

    uint64_t                   get_milli_seconds() const override;

    void                       get_window_bounds(GLFW_Rect& rect) const override;

    void                       set_use_window_frame(bool use_window_frame) const override;

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

    static bool         use_window_frame_;

    static std::string  system_backend_id_;

    static GLFW_System* system_;

    MEM_CXX_CLASS_ALLOC_FUNCS("GLFW:GLFW_System")
};
} // namespace vektor