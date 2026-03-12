#pragma once

#include "VPI_Types.h"
#include "intern/VPI_Event.hh"

namespace vpi {
class VPI_IEventConsumer {
 public:
  virtual VPI_TSuccess consume_event(VPI_Event *event) const noexcept = 0;

 protected:
  explicit VPI_IEventConsumer() = default;
  virtual ~VPI_IEventConsumer() = default;
};
}  // namespace vpi
