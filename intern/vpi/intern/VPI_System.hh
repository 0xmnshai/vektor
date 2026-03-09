#pragma once

#include <QElapsedTimer>

#include "VPI_EventManager.hh"
#include "VPI_ISystem.h"
#include "VPI_WindowManager.hh"
#include "intern/VPI_QtWindow.hh"

namespace vpi {
class VPI_System : public VPI_ISystem {
 public:
  explicit VPI_System();
  ~VPI_System() override;

  [[nodiscard]] VPI_IWindow *create_window(char const *title,
                                           int32_t left,
                                           int32_t top,
                                           uint32_t width,
                                           uint32_t height,
                                           VPI_IWindow const *parent_window) noexcept override;

  VPI_TSuccess init() override;

  VPI_TSuccess exit(bool &is_running) override;

  [[nodiscard]] uint64_t get_milliseconds() const noexcept override;

  [[nodiscard]] VPI_IWindow *get_window_under_cursor(int32_t x, int32_t y) const noexcept override;

  [[nodiscard]] inline VPI_WindowManager const &get_window_manager() const noexcept
  {
    return *window_manager_;
  }

  [[nodiscard]] inline VPI_EventManager const &get_event_manager() const noexcept
  {
    return *event_manager_;
  }

 private:
  VPI_WindowManager *window_manager_;
  VPI_EventManager *event_manager_;
  VPI_QtWindow *qt_window_;
  QApplication *qt_app_;
  QElapsedTimer timer_;
};

}  // namespace vpi
