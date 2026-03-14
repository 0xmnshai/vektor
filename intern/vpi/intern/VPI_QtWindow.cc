#include "VPI_QtWindow.hh"
#include "VPI_EventManager.hh"
#include "VPI_WindowManager.hh"
#include "intern/VPI_GLWidget.hh"

namespace vpi {

VPI_QtWindow::VPI_QtWindow()
    : gl_widget_(new VPI_GLWidget()),
      window_manager_(new VPI_WindowManager()),
      event_manager_(new VPI_EventManager()),
      style_manager_(new qt::ui::UI_style_manager()),
      widget_(nullptr)
{
  setCentralWidget(gl_widget_);
}

VPI_QtWindow::~VPI_QtWindow()
{
  delete gl_widget_;
  delete window_manager_;
  delete event_manager_;
}

void VPI_QtWindow::create_window(char const *title,
                                 int32_t left,
                                 int32_t top,
                                 uint32_t width,
                                 uint32_t height,
                                 VPI_IWindow const *parent_window) noexcept
{
  setWindowTitle(title);
  setGeometry(left, top, static_cast<int>(width), static_cast<int>(height));
  show();

  setup_menus();
}

VPI_TSuccess VPI_QtWindow::dispose() noexcept
{
  this->close();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::get_title(char const **title) const noexcept
{
  static std::string t;
  t = windowTitle().toStdString();
  *title = t.c_str();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::set_title(char const *title) noexcept
{
  setWindowTitle(title);
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::get_position(int32_t *left, int32_t *top) const noexcept
{
  *left = x();
  *top = y();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::set_position(int32_t left, int32_t top) noexcept
{
  move(left, top);
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::get_size(uint32_t *width, uint32_t *height) const noexcept
{
  *width = this->width();
  *height = this->height();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::set_size(uint32_t width, uint32_t height) noexcept
{
  resize(static_cast<int>(width), static_cast<int>(height));
  return VPI_kSuccess;
}

VPI_TWindowState VPI_QtWindow::get_state() const noexcept
{
  if (isMinimized())
    return VPI_kWindowStateMinimized;
  if (isMaximized())
    return VPI_kWindowStateMaximized;
  if (isFullScreen())
    return VPI_kWindowStateFullScreen;
  return VPI_kWindowStateNormal;
}

VPI_TSuccess VPI_QtWindow::set_state(VPI_TWindowState state) noexcept
{
  switch (state) {
    case VPI_kWindowStateMinimized:
      showMinimized();
      break;
    case VPI_kWindowStateMaximized:
      showMaximized();
      break;
    case VPI_kWindowStateFullScreen:
      showFullScreen();
      break;
    case VPI_kWindowStateNormal:
      showNormal();
      break;
    default:
      return VPI_kFailure;
  }
  return VPI_kSuccess;
}

bool VPI_QtWindow::process_events(bool /*wait_for_event*/) noexcept
{
  QCoreApplication::processEvents();
  (void)event_manager_->dispatch_events();
  return isVisible();
}

VPI_TSuccess VPI_QtWindow::get_native_handle(void const **handle) const noexcept
{
  *handle = reinterpret_cast<void const *>(winId());
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::add_event_consumer(VPI_QtEventConsumer *consumer) noexcept
{
  return event_manager_->add_consumer(consumer);
}

VPI_TSuccess VPI_QtWindow::get_cursor_position(int32_t *x, int32_t *y) const noexcept
{
  QPoint p = mapFromGlobal(QCursor::pos());
  *x = p.x();
  *y = p.y();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_QtWindow::set_cursor_position(int32_t x, int32_t y) noexcept
{
  QCursor::setPos(mapToGlobal(QPoint(x, y)));
  return VPI_kSuccess;
}

void VPI_QtWindow::get_window_bounds(VPI_Rect &bounds) const
{
  QRect r = frameGeometry();
  bounds.set(r.x(), r.y(), r.width(), r.height());
}

void VPI_QtWindow::get_client_bounds(VPI_Rect &bounds) const
{
  QRect r = geometry();
  bounds.set(r.x(), r.y(), r.width(), r.height());
}

VPI_WindowManager const &VPI_QtWindow::get_window_manager() const noexcept
{
  return *window_manager_;
}

VPI_EventManager const &VPI_QtWindow::get_event_manager() const noexcept
{
  return *event_manager_;
}

void VPI_QtWindow::setup_menus() {}

void VPI_QtWindow::setup_docks() {}
}  // namespace vpi
