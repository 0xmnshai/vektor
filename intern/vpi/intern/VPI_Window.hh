#pragma once

#include "../VPI_IWindow.h"

namespace vpi {
class VPI_Window : public VPI_IWindow {
 public:
  VPI_Window() = default;
  ~VPI_Window() override = default;

  void create_window(char const *title,
                     int32_t left,
                     int32_t top,
                     uint32_t width,
                     uint32_t height,
                     VPI_IWindow const *parent_window) noexcept override = 0;

  [[nodiscard]] VPI_TSuccess dispose() noexcept override = 0;

  [[nodiscard]] VPI_TSuccess get_title(char const **title) const noexcept override = 0;

  [[nodiscard]] VPI_TSuccess set_title(char const *title) noexcept override = 0;

  [[nodiscard]] VPI_TSuccess get_position(int32_t *left, int32_t *top) const noexcept override = 0;

  [[nodiscard]] VPI_TSuccess set_position(int32_t left, int32_t top) noexcept override = 0;

  [[nodiscard]] VPI_TSuccess get_size(uint32_t *width,
                                      uint32_t *height) const noexcept override = 0;

  [[nodiscard]] VPI_TSuccess set_size(uint32_t width, uint32_t height) noexcept override = 0;

  [[nodiscard]] VPI_TWindowState get_state() const noexcept override = 0;

  [[nodiscard]] VPI_TSuccess set_state(VPI_TWindowState state) noexcept override = 0;

  [[nodiscard]] bool process_events(bool wait_for_event) noexcept override = 0;

  [[nodiscard]] VPI_TSuccess get_native_handle(void const **handle) const noexcept override = 0;

  VPI_TSuccess add_event_consumer(VPI_IEventConsumer *consumer) noexcept override = 0;

  [[nodiscard]] VPI_TSuccess get_cursor_position(int32_t *x,
                                                 int32_t *y) const noexcept override = 0;

  [[nodiscard]] VPI_TSuccess set_cursor_position(int32_t x, int32_t y) noexcept override = 0;

  virtual void get_window_bounds(VPI_Rect &bounds) const override = 0;

  virtual void get_client_bounds(VPI_Rect &bounds) const override = 0;

 protected:
};
}  // namespace vpi
