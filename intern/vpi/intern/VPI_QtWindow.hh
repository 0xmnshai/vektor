#pragma once

#include <QApplication>
#include <QElapsedTimer>
#include <QMainWindow>
#include <QOpenGLContext>
#include <QPoint>
#include <QSize>
#include <QSurfaceFormat>
#include <QWidget>
#include <new>

#include "../../intern/gaurdalloc/MEM_gaurdalloc.h"
#include "../../intern/qt/ui/UI_style_manager.h"
#include "../intern/VPI_EventManager.hh"
#include "../intern/VPI_GLWidget.hh"
#include "../intern/VPI_QtEventConsumer.hh"
#include "VPI_Window.hh"
#include "VPI_WindowManager.hh"

namespace vpi {
class VPI_QtWindow : public QMainWindow, public VPI_Window {
  Q_OBJECT
 public:
  explicit VPI_QtWindow();
  explicit VPI_QtWindow(QWidget *widget);
  ~VPI_QtWindow() override;

  void *operator new(size_t size)
  {
    return MEM_mallocN(size, "VPI_QtWindow");
  }

  void *operator new(size_t size, std::align_val_t alignment)
  {
    return mem_guarded::internal::mem_mallocN_aligned_ex(
        size,
        static_cast<size_t>(alignment),
        "VPI_QtWindow",
        mem_guarded::internal::DestructorType::Trivial);
  }

  void *operator new(size_t /*size*/, void *ptr) noexcept
  {
    return ptr;
  }

  void operator delete(void *ptr)
  {
    MEM_freeN(ptr);
  }

  void operator delete(void *ptr, std::align_val_t /*alignment*/)
  {
    MEM_freeN(ptr);
  }

  void operator delete(void * /*ptr*/, void * /*place*/) noexcept {}

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

  VPI_TSuccess add_event_consumer(VPI_QtEventConsumer *consumer) noexcept override;

  [[nodiscard]] VPI_TSuccess get_cursor_position(int32_t *x, int32_t *y) const noexcept override;

  [[nodiscard]] VPI_TSuccess set_cursor_position(int32_t x, int32_t y) noexcept override;

  void get_window_bounds(VPI_Rect &bounds) const override;

  void get_client_bounds(VPI_Rect &bounds) const override;

  [[nodiscard]] VPI_WindowManager const &get_window_manager() const noexcept;
  [[nodiscard]] VPI_WindowManager &get_window_manager() noexcept;

  [[nodiscard]] VPI_EventManager const &get_event_manager() const noexcept;

  [[nodiscard]] inline QWidget *get_widget() const noexcept
  {
    return widget_;
  }

  inline void set_widget(QWidget *widget)
  {
    widget_ = widget;
  }

  void setup_menus();
  void setup_docks();
  QDockWidget *setup_new_window_with_viewport();

 protected:
  VPI_GLWidget *gl_widget_;
  VPI_WindowManager *window_manager_;
  VPI_EventManager *event_manager_;
  qt::ui::UI_style_manager *style_manager_;
  QWidget *widget_;

  friend class VPI_System;
};
}  // namespace vpi
