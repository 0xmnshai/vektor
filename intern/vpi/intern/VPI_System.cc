#include <vector>

#include "VPI_IWindow.h"
#include "VPI_Rect.h"
#include "VPI_System.hh"

namespace vpi {
VPI_System::VPI_System()
    : window_manager_(nullptr), event_manager_(nullptr), qt_window_(nullptr), qt_app_(nullptr)
{
}

VPI_System::~VPI_System() = default;

VPI_IWindow *VPI_System::create_window(char const *title,
                                       int32_t left,
                                       int32_t top,
                                       uint32_t width,
                                       uint32_t height,
                                       VPI_IWindow const *parent_window) noexcept
{
  qt_window_->create_window(title, left, top, width, height, parent_window);
  qt_window_->show();
  return qt_window_;
}

VPI_TSuccess VPI_System::init()
{
  static int argc = 1;
  static char *argv[] = {(char *)"vektor", nullptr};

  QSurfaceFormat format;
  format.setVersion(6, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  QSurfaceFormat::setDefaultFormat(format);

  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

  qt_app_ = new QApplication(argc, argv);
  qt_app_->setStyleSheet(vpi::get_qt_style());

  window_manager_ = new VPI_WindowManager();
  event_manager_ = new VPI_EventManager();
  qt_window_ = new VPI_QtWindow();
  timer_.start();

  return VPI_kSuccess;
}

VPI_TSuccess VPI_System::exit(bool &is_running)
{
  delete window_manager_;
  delete event_manager_;
  delete qt_window_;
  delete qt_app_;
  timer_.restart();
  is_running = false;

  return VPI_kSuccess;
}

uint64_t VPI_System::get_milliseconds() const noexcept
{
  return (uint64_t)timer_.elapsed();
}

VPI_IWindow *VPI_System::get_window_under_cursor(int32_t x, int32_t y) const noexcept
{
  std::vector<VPI_IWindow *> windows = window_manager_->get_windows();
  std::vector<VPI_IWindow *>::reverse_iterator iwindow_iter;

  for (iwindow_iter = windows.rbegin(); iwindow_iter != windows.rend(); ++iwindow_iter) {
    VPI_IWindow *win = *iwindow_iter;

    if (win->get_state() == VPI_kWindowStateMinimized) {
      continue;
    }

    VPI_Rect bounds;

    win->get_client_bounds(bounds);

    if (bounds.is_inside(x, y)) {
      return win;
    }
  }

  return nullptr;
}
}  // namespace vpi
