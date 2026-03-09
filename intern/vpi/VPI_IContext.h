#pragma once

#include "VPI_IWindow.h"
#include "VPI_Types.h"

// Context Represents - openGL, Metal-Cpp ( for now ) and Vulkan ( later )
namespace vpi {
class VPI_IContext {
 public:
  virtual ~VPI_IContext() = default;

  [[nodiscard]] virtual VPI_TSuccess create() const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess dispose() const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess get_native_handle(void const **handle) const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess get_window(VPI_IWindow const **window) const noexcept = 0;
};
}  // namespace vpi
