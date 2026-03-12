#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "VPI_EventManager.hh"
#include "VPI_IEventConsumer.h"
#include "VPI_Types.h"

#include "../../clog/CLG_log.h"

CLG_LOGREF_DECLARE_GLOBAL(vpi_events, "vpi.events");

namespace vpi {

VPI_EventManager::~VPI_EventManager()
{
  consumers_.clear();
}

uint32_t VPI_EventManager::get_num_events() const noexcept
{
  return static_cast<uint32_t>(events_.size());
}

uint32_t VPI_EventManager::get_num_events(VPI_EventType type) const noexcept
{
  uint32_t num_events = 0;
  for (auto const &event : events_) {
    if (event->get_type() == type) {
      num_events++;
    }
  }
  return num_events;
}

VPI_TSuccess VPI_EventManager::push_event(std::unique_ptr<VPI_Event const> event)
{
  events_.push_back(std::move(event));
  num_events_++;
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::pop_event(std::unique_ptr<VPI_Event const> &event)
{
  if (events_.empty()) {
    return VPI_kFailure;
  }
  event = std::move(events_.front());
  events_.pop_front();
  num_events_--;
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispatch_event(VPI_Event *event)
{
  CLOG_INFO(vpi_events, "Dispatching event type: %d", (int)event->get_type());

  for (auto const *consumer : consumers_) {
    if (consumer->consume_event(event) == VPI_kSuccess) {
      if (event->is_consumed() == VPI_kSuccess) {
        CLOG_DEBUG(vpi_events, "Event consumed by: %p", (void *)consumer);
        // VPI_kWindowClose
        if (event->get_type() == VPI_kWindowClose) {
          // we will use qt event consumer here and will close the system 
        }
        break;
      }
    }
  }
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispatch_event()
{
  if (events_.empty()) {
    return VPI_kFailure;
  }

  std::unique_ptr<VPI_Event const> event = std::move(events_.front());
  events_.pop_front();

  VPI_Event const *event_ptr = event.get();
  (void)dispatch_event(const_cast<VPI_Event *>(event_ptr));
  handled_events_.push_back(std::move(event));
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispatch_events()
{
  while (!events_.empty()) {
    (void)dispatch_event();
  }

  (void)dispose_event();

  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispose_event()
{
  handled_events_.clear();
  events_.clear();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::add_consumer(VPI_IEventConsumer const *consumer)
{
  consumers_.push_back(consumer);
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::remove_consumer(VPI_IEventConsumer const *consumer)
{
  auto it = std::find(consumers_.begin(), consumers_.end(), consumer);
  if (it != consumers_.end()) {
    consumers_.erase(it);
    return VPI_kSuccess;
  }
  return VPI_kFailure;
}

}  // namespace vpi
