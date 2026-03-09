#pragma once

#include <vector>

#include "VPI_IWindow.h"
#include "VPI_Types.h"

namespace vpi {
class VPI_WindowManager {
 public:
  explicit VPI_WindowManager() = default;
  ~VPI_WindowManager() = default;

  VPI_TSuccess add_window(VPI_IWindow *window);

  VPI_TSuccess remove_window(VPI_IWindow const *window);

  [[nodiscard]] std::vector<VPI_IWindow *> const &get_windows() const;

  [[nodiscard]] VPI_IWindow const *get_active_window() const;

 private:
std::vector<VPI_IWindow *> windows_;
  VPI_IWindow *active_window_ = nullptr;
};
}  // namespace vpi
