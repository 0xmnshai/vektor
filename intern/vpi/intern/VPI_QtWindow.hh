#pragma once

#include <QApplication>
#include <QElapsedTimer>
#include <QMainWindow>
#include <QOpenGLContext>
#include <QPoint>
#include <QSize>
#include <QSurfaceFormat>
#include <QWidget>

#include "VPI_Window.hh"

namespace vpi {
class VPI_QtWindow : public VPI_Window, public QMainWindow {
 public:
  explicit VPI_QtWindow();
  ~VPI_QtWindow() override;

  void create_window(char const *title,
                     int32_t left,
                     int32_t top,
                     uint32_t width,
                     uint32_t height,
                     VPI_IWindow const *parent_window) noexcept override;

  [[nodiscard]] VPI_TSuccess dispose() noexcept override;

  [[nodiscard]] VPI_TSuccess get_title(char const **title) const noexcept override;

  [[nodiscard]] VPI_TSuccess set_title(char const *title) noexcept override;

  [[nodiscard]] VPI_TSuccess get_position(int32_t *left, int32_t *top) const noexcept override;

  [[nodiscard]] VPI_TSuccess set_position(int32_t left, int32_t top) noexcept override;

  [[nodiscard]] VPI_TSuccess get_size(uint32_t *width, uint32_t *height) const noexcept override;

  [[nodiscard]] VPI_TSuccess set_size(uint32_t width, uint32_t height) noexcept override;

  [[nodiscard]] VPI_TWindowState get_state() const noexcept override;

  [[nodiscard]] VPI_TSuccess set_state(VPI_TWindowState state) noexcept override;

  [[nodiscard]] bool process_events(bool wait_for_event) noexcept override;

  [[nodiscard]] VPI_TSuccess get_native_handle(void const **handle) const noexcept override;

  VPI_TSuccess add_event_consumer(VPI_IEventConsumer *consumer) noexcept override;

  [[nodiscard]] VPI_TSuccess get_cursor_position(int32_t *x, int32_t *y) const noexcept override;

  [[nodiscard]] VPI_TSuccess set_cursor_position(int32_t x, int32_t y) noexcept override;

  void get_window_bounds(VPI_Rect &bounds) const override;

  void get_client_bounds(VPI_Rect &bounds) const override;
};

QString get_qt_style();

}  // namespace vpi
