#pragma once

#include "VPI_Types.h"

namespace vpi {
class VPI_IEvent {
 public:
  virtual ~VPI_IEvent() = default;
  [[nodiscard]] virtual VPI_EventType get_type() const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess is_consumed() const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess consume() const noexcept = 0;
};
}  // namespace vpi
