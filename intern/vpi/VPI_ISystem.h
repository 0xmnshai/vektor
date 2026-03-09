#pragma once

#include <cstdint>

#include "VPI_IWindow.h"
#include "VPI_Types.h"

namespace vpi {
class VPI_ISystem {
 public:
  static VPI_TSuccess create(bool verbose = false, bool background = false);

  static VPI_TSuccess create_background();

  static VPI_TSuccess dispose();

  static VPI_TSuccess dispose_background();

  static VPI_ISystem *get();

  static VPI_ISystem *get_background();

  virtual VPI_IWindow *create_window(char const *title,
                                     int32_t left,
                                     int32_t top,
                                     uint32_t width,
                                     uint32_t height,
                                     VPI_IWindow const *parent_window) noexcept = 0;

  virtual VPI_TSuccess init() = 0;

  virtual VPI_TSuccess exit(bool &is_running) = 0;

  [[nodiscard]] virtual uint64_t get_milliseconds() const noexcept = 0;

  [[nodiscard]] virtual VPI_IWindow *get_window_under_cursor(int32_t x,
                                                             int32_t y) const noexcept = 0;

 protected:
  explicit VPI_ISystem() = default;
  virtual ~VPI_ISystem() = default;
};
}  // namespace vpi
