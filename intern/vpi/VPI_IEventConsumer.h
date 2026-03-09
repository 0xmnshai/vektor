#pragma once

#include "VPI_IEvent.h"
#include "VPI_Types.h"

namespace vpi {
class VPI_IEventConsumer {
 public:
  virtual VPI_TSuccess consume_event(VPI_IEvent *event) const noexcept = 0;

 protected:
  explicit VPI_IEventConsumer() = default;
  virtual ~VPI_IEventConsumer() = default;
};
}  // namespace vpi
