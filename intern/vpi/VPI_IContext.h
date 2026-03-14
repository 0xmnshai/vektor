#pragma once

// Context Represents - openGL, Metal-Cpp ( for now ) and Vulkan ( later )
#include "VPI_Types.h"
namespace vpi {
class VPI_IContext {
 public:
  virtual ~VPI_IContext() = default;

  static VPI_IContext *get_active_context();

  [[nodiscard]] virtual VPI_TSuccess init_context() const = 0;

  [[nodiscard]] virtual VPI_TSuccess release_context() const = 0;

  [[nodiscard]] virtual VPI_TSuccess release_native_handles() const = 0;

  virtual void resize_context(uint32_t width, uint32_t height) const = 0;
};
}  // namespace vpi
