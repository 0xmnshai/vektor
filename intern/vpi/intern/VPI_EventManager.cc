#include "VPI_EventManager.hh"

namespace vpi {

uint32_t VPI_EventManager::get_num_events(VPI_EventType type) const noexcept
{
  return static_cast<uint32_t>(events_.size());
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

VPI_TSuccess VPI_EventManager::dispatch_event()
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispatch_events()
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_EventManager::dispatch_event(VPI_EventType const *type)
{
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
