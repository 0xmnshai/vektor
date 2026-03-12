#pragma once

#include <vector>

#include "VPI_Window.hh"
#include "VPI_Types.h"

namespace vpi {
class VPI_WindowManager {
 public:
  explicit VPI_WindowManager() = default;
  ~VPI_WindowManager() = default;

  VPI_TSuccess add_window(VPI_Window *window);

  VPI_TSuccess remove_window(VPI_Window const *window);

  [[nodiscard]] std::vector<VPI_Window *> const &get_windows() const;

  [[nodiscard]] VPI_Window const *get_active_window() const;

 private:
  std::vector<VPI_Window *> windows_;
  VPI_Window *active_window_ = nullptr;
};
}  // namespace vpi
