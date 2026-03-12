#include "VPI_Event.hh"
#include "../../intern/clog/CLG_log.h"

namespace vpi {

CLG_LOGREF_DECLARE_GLOBAL(Event, "vpi.event");

[[nodiscard]] VPI_TSuccess VPI_Event::consume() const noexcept
{
  if (consumed_) {
    CLOG_WARN(Event, "Attempted to consume already-consumed event (type=%d)", (int)type_);
    return VPI_kSuccess;
  }

  consumed_ = true;

  CLOG_INFO(Event, "Event consumed (type=%d)", (int)type_);

  return VPI_kSuccess;
}

}  // namespace vpi
