#include "VPI_Event.hh"

namespace vpi {
[[nodiscard]] VPI_TSuccess VPI_Event::consume() const noexcept
{
  return VPI_kSuccess;
};

}  // namespace vpi
