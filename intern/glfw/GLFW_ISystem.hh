#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>

#include "GLFW_IEventConsumer.hh"
#include "GLFW_rect.hh"

namespace vektor
{
typedef void (*GLFW_TBackTraceFn)(FILE* file_handle);

enum GLFW_TSuccess
{
    GLFW_kFailure = 0,
    GLFW_kSuccess
};

class GLFW_ISystem
{
public:
    virtual GLFW_TSuccess     create_system(bool verbose    = true,
                                            bool background = false) = 0;

    virtual GLFW_TSuccess     create_system_background()             = 0;

    virtual GLFW_TSuccess     destroy_system() const                 = 0;

    static GLFW_TSuccess      destroy_system_background();

    static GLFW_ISystem*      get_system();

    virtual void              get_window_bounds(GLFW_Rect& rect) const = 0;

    static const char*        get_system_backend();

    virtual void              set_use_window_frame(bool use_window_frame) const = 0;

    virtual uint64_t          get_milli_seconds() const                         = 0;

    virtual GLFW_TBackTraceFn get_backtrace_fn();

    static void               set_backtrace_fn(GLFW_TBackTraceFn backtrace_fn) { backtrace_fn_ = backtrace_fn; };

    virtual GLFW_TSuccess     dispose_window(GLFW_IWindow* window)                 = 0;

    virtual void              dispatch_events()                                    = 0;

    virtual bool              valid_window(GLFW_IWindow* window) const             = 0;

    virtual GLFW_TSuccess     add_event_consumer(GLFW_IEventConsumer* consumer)    = 0;

    virtual GLFW_TSuccess     remove_event_consumer(GLFW_IEventConsumer* consumer) = 0;

    virtual GLFW_TSuccess     push_event(std::unique_ptr<const GLFW_IEvent> event) = 0;

protected:
    virtual GLFW_TSuccess init() = 0;

    virtual GLFW_TSuccess exit() = 0;

    static GLFW_ISystem*  system_;

    static bool           use_window_frame_;

protected:
    GLFW_ISystem();
    virtual ~GLFW_ISystem() = default;
    static GLFW_TBackTraceFn backtrace_fn_;
};
} // namespace vektor