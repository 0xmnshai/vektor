#pragma once

#include "VPI_IEvent.h"

namespace vpi {
class VPI_Event : public VPI_IEvent {
  virtual ~VPI_Event() = default;

  [[nodiscard]] inline VPI_EventType get_type() const noexcept override
  {
    return type_;
  };

  [[nodiscard]] VPI_TSuccess is_consumed() const noexcept override = 0;

  [[nodiscard]] VPI_TSuccess consume() const noexcept override = 0;

 protected:
  VPI_EventType type_;
};
}  // namespace vpi
