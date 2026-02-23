#include "GLFW_event_manager.hh"
#include "GLFW_IEvent.hh"
#include "GLFW_IEventConsumer.hh"
#include "GLFW_ISystem.hh"

namespace vektor
{
GLFW_EventManager::GLFW_EventManager() = default;

GLFW_EventManager::~GLFW_EventManager()
{
    dispose_event();

    TConsumerVector::iterator it = consumers_.begin();

    while (it != consumers_.end())
    {
        GLFW_IEventConsumer* consumer = *it;
        delete consumer;
        it = consumers_.erase(it);
    }
}

uint32_t GLFW_EventManager::get_num_events() const
{
    return events_.size();
}

uint32_t GLFW_EventManager::get_num_events(GLFW_TEventType type) const
{
    uint32_t                    numEvents = 0;
    TEventStack::const_iterator p_iter;

    for (p_iter = events_.begin(); p_iter != events_.end(); ++p_iter)
    {
        if ((*p_iter)->get_type() == type)
        {
            numEvents++;
        }
    }
    return numEvents;
};

GLFW_TSuccess GLFW_EventManager::push_event(std::unique_ptr<const GLFW_IEvent> event)
{
    GLFW_TSuccess success;
    if (events_.size() < events_.max_size())
    {
        events_.push_front(std::move(event));
        success = GLFW_kSuccess;
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

void GLFW_EventManager::dispatch_event(const GLFW_IEvent* event)
{
    TConsumerVector::iterator iter;

    for (iter = consumers_.begin(); iter != consumers_.end(); ++iter)
    {
        (*iter)->process_event(event);
    }
}

void GLFW_EventManager::dispatch_event()
{
    std::unique_ptr<const GLFW_IEvent> event = std::move(events_.back());
    events_.pop_back();
    const GLFW_IEvent* event_ptr = event.get();
    handled_events_.push_back(std::move(event));
    dispatch_event(event_ptr);
}

void GLFW_EventManager::dispatch_events()
{
    while (!events_.empty())
    {
        dispatch_event();
    }

    dispatch_event();
}

GLFW_TSuccess GLFW_EventManager::add_consumer(GLFW_IEventConsumer* consumer)
{
    GLFW_TSuccess                   success;
    TConsumerVector::const_iterator iter = std::find(consumers_.begin(), consumers_.end(), consumer);

    if (iter == consumers_.end())
    {
        consumers_.push_back(consumer);
        success = GLFW_kSuccess;
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

GLFW_TSuccess GLFW_EventManager::remove_consumer(GLFW_IEventConsumer* consumer)
{
    GLFW_TSuccess             success;

    TConsumerVector::iterator iter = std::find(consumers_.begin(), consumers_.end(), consumer);

    if (iter != consumers_.end())
    {
        consumers_.erase(iter);
        success = GLFW_kSuccess;
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

void GLFW_EventManager::remove_window_events(const GLFW_IWindow* window)
{
    TEventStack::iterator iter;
    iter = events_.begin();
    while (iter != events_.end())
    {
        std::unique_ptr<const GLFW_IEvent>& event = *iter;
        if (event->get_window() == window)
        {
            event.reset();
            events_.erase(iter);
            iter = events_.begin();
        }
        else
        {
            ++iter;
        }
    }
}

void GLFW_EventManager::dispose_event()
{
    while (handled_events_.empty() == false)
    {
        handled_events_[0].reset();
        handled_events_.pop_front();
    }

    while (events_.empty() == false)
    {
        events_[0].reset();
        events_.pop_front();
    }
}
} // namespace vektor