#pragma once

#include <cstdint>
#include <deque>  // IWYU pragma: keep
#include <vector> // IWYU pragma: keep

#include "GLFW_IEvent.hh"
#include "GLFW_IEventConsumer.hh"
#include "GLFW_ISystem.hh"
#include "GLFW_types.h"

namespace vektor
{
class GLFW_EventManager
{
public:
    uint32_t      get_num_events() const;

    uint32_t      get_num_events(GLFW_TEventType) const;

    GLFW_TSuccess push_event(std::unique_ptr<const GLFW_IEvent> event);

    void          dispatch_event(const GLFW_IEvent* event);

    void          dispatch_event();

    void          dispatch_events();

    GLFW_TSuccess add_consumer(GLFW_IEventConsumer* consumer);

    GLFW_TSuccess remove_consumer(GLFW_IEventConsumer* consumer);

    void          remove_window_events(const GLFW_IWindow* window);

    void          dispose_event();

    GLFW_EventManager();

    ~GLFW_EventManager();

protected:
    using TEventStack = std::deque<std::unique_ptr<const GLFW_IEvent>>;

    TEventStack events_;
    TEventStack handled_events_;

    using TConsumerVector = std::vector<GLFW_IEventConsumer*>;
    using TEventVector    = std::vector<GLFW_IEvent*>;

    TConsumerVector consumers_;

    MEM_CXX_CLASS_ALLOC_FUNCS("GLFW:GLFW_EventManager")
};
} // namespace vektor
