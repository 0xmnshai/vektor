#pragma once

#include <cstdint>

#include "VPI_Rect.h"
#include "VPI_Types.h"
#include "intern/VPI_QtEventConsumer.hh"

namespace vpi {
class VPI_IWindow {
 public:
  virtual void create_window(char const *title,
                             int32_t left,
                             int32_t top,
                             uint32_t width,
                             uint32_t height,
                             VPI_IWindow const *parent_window) noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess dispose() noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess get_title(char const **title) const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess set_title(char const *title) noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess get_position(int32_t *left, int32_t *top) const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess set_position(int32_t left, int32_t top) noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess get_size(uint32_t *width,
                                              uint32_t *height) const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess set_size(uint32_t width, uint32_t height) noexcept = 0;

  [[nodiscard]] virtual VPI_TWindowState get_state() const noexcept = 0;

  // we will use Qt to check window minimise or other things
  [[nodiscard]] virtual VPI_TSuccess set_state(VPI_TWindowState state) noexcept = 0;

  [[nodiscard]] virtual bool process_events(bool wait_for_event) noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess get_native_handle(void const **handle) const noexcept = 0;

  virtual VPI_TSuccess add_event_consumer(VPI_QtEventConsumer *consumer) noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess get_cursor_position(int32_t *x,
                                                         int32_t *y) const noexcept = 0;

  [[nodiscard]] virtual VPI_TSuccess set_cursor_position(int32_t x, int32_t y) noexcept = 0;

  virtual void get_window_bounds(VPI_Rect &bounds) const = 0;

  virtual void get_client_bounds(VPI_Rect &bounds) const = 0;

 protected:
  explicit VPI_IWindow() = default;
  virtual ~VPI_IWindow() = default;
  static VPI_TWindowState state_;
};
}  // namespace vpi
