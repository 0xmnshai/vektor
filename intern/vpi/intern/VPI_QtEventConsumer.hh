#pragma once

#include "VPI_IEventConsumer.h"
namespace vpi {
class VPI_QtEventConsumer : public VPI_IEventConsumer {
 public:
  VPI_TSuccess consume_event(VPI_IEvent *event) const noexcept override;
 protected:
  explicit VPI_QtEventConsumer() = default;
  ~VPI_QtEventConsumer() override = default;
};
}  // namespace vpi
