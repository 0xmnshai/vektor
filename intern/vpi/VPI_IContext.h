#pragma once

// Context Represents - openGL, Metal-Cpp ( for now ) and Vulkan ( later )
#include "VPI_Types.h"
namespace vpi {
class VPI_IContext {
 public:
  virtual ~VPI_IContext() = default;

  static VPI_IContext *get_active_context();

  [[nodiscard]] virtual VPI_TSuccess activate_context() const = 0;

  [[nodiscard]] virtual VPI_TSuccess release_context() const = 0;
};
}  // namespace vpi
