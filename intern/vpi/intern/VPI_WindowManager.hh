#pragma once

#include <vector>
#include "../VPI_Types.h"

namespace vpi {
class VPI_QtWindow;

class VPI_WindowManager {
 public:
  explicit VPI_WindowManager() = default;
  ~VPI_WindowManager() = default;

  VPI_TSuccess add_window(VPI_QtWindow *window);

  VPI_TSuccess remove_window(VPI_QtWindow const *window);

  [[nodiscard]] std::vector<VPI_QtWindow *> const &get_windows() const;

  [[nodiscard]] VPI_QtWindow const *get_active_window() const;

 private:
  std::vector<VPI_QtWindow *> windows_;
  VPI_QtWindow *active_window_ = nullptr;
};
}  // namespace vpi
