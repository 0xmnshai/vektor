#pragma once

#include <cstdint>
#include <deque>
#include <memory>

#include "VPI_Event.hh"
#include "VPI_IEventConsumer.h"
#include "VPI_Types.h"

namespace vpi {
class VPI_EventManager {
 public:
  explicit VPI_EventManager() = default;
  ~VPI_EventManager();

  [[nodiscard]] uint32_t get_num_events() const noexcept;

  [[nodiscard]] uint32_t get_num_events(VPI_EventType type) const noexcept;

  [[nodiscard]] VPI_TSuccess push_event(std::unique_ptr<VPI_Event const> event);

  [[nodiscard]] VPI_TSuccess pop_event(std::unique_ptr<VPI_Event const> &event);

  [[nodiscard]] VPI_TSuccess dispatch_event();

  [[nodiscard]] VPI_TSuccess dispatch_events();

  [[nodiscard]] VPI_TSuccess dispose_event();

  [[nodiscard]] VPI_TSuccess dispatch_event(VPI_Event *event);

  [[nodiscard]] VPI_TSuccess add_consumer(VPI_IEventConsumer const *consumer);

  [[nodiscard]] VPI_TSuccess remove_consumer(VPI_IEventConsumer const *consumer);

 private:
  uint32_t num_events_ = 0;

  std::deque<std::unique_ptr<VPI_Event const>> events_;
  std::deque<std::unique_ptr<VPI_Event const>> handled_events_;

  std::vector<VPI_IEventConsumer const *> consumers_;
};
}  // namespace vpi
