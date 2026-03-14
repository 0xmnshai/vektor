#include "VPI_WindowManager.hh"
#include "VPI_QtWindow.hh"
#include <algorithm>

namespace vpi {

VPI_TSuccess VPI_WindowManager::add_window(VPI_QtWindow *window)
{
  windows_.push_back(window);
  active_window_ = window;
  return VPI_kSuccess;
}

VPI_TSuccess VPI_WindowManager::remove_window(VPI_QtWindow const *window)
{
  auto it = std::find(windows_.begin(), windows_.end(), window);
  if (it != windows_.end()) {
    windows_.erase(it);
    if (active_window_ == window) {
      active_window_ = windows_.empty() ? nullptr : windows_.back();
    }
    return VPI_kSuccess;
  }
  return VPI_kFailure;
}

std::vector<VPI_QtWindow *> const &VPI_WindowManager::get_windows() const
{
  return windows_;
}

VPI_QtWindow const *VPI_WindowManager::get_active_window() const
{
  return active_window_;
}

}  // namespace vpi
