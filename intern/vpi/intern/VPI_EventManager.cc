#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "VPI_EventManager.hh"
#include "VPI_IEvent.h"
#include "VPI_IEventConsumer.h"
#include "VPI_Types.h"

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

VPI_TSuccess VPI_EventManager::push_event(std::unique_ptr<VPI_IEvent const> event)
{
  events_.push_back(std::move(event));
  num_events_++;
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::pop_event(std::unique_ptr<VPI_IEvent const> &event)
{
  if (events_.empty()) {
    return VPI_kFailure;
  }
  event = std::move(events_.front());
  events_.pop_front();
  num_events_--;
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispatch_event(VPI_IEvent *event)
{
  std::vector<VPI_IEventConsumer const *>::iterator consumer_iter;

  for (consumer_iter = consumers_.begin(); consumer_iter != consumers_.end(); ++consumer_iter) {
    (*consumer_iter)->consume_event(event);
  }
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispatch_event()
{
  if (events_.empty()) {
    return VPI_kFailure;
  }

  std::unique_ptr<VPI_IEvent const> event = std::move(events_.back());
  events_.pop_back();

  VPI_IEvent const *event_ptr = event.get();
  (void)dispatch_event(const_cast<VPI_IEvent *>(event_ptr));
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
  while (handled_events_.empty() == false) {
    handled_events_[0].reset();
    handled_events_.pop_front();
  }

  while (events_.empty() == false) {
    events_[0].reset();
    events_.pop_front();
  }

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
