#include <qwidget.h>
#include <vector>

#include "../../source/editor/windowmanager/wm_event_types.h"
#include "VPI_System.hh"
#include "intern/VPI_QtWindow.hh"
#include "intern/CLG_init.hh"

namespace vpi {

static VPI_ISystem *g_vpi_system = nullptr;
static VPI_ISystem *g_vpi_system_background = nullptr;

VPI_TSuccess VPI_ISystem::create(bool /*verbose*/, bool background)
{
  if (background) {
    if (g_vpi_system_background) {
      return VPI_kFailure;
    }
    g_vpi_system_background = new VPI_System();
    return VPI_kSuccess;
  }

  if (g_vpi_system) {
    return VPI_kFailure;
  }
  g_vpi_system = new VPI_System();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_ISystem::create_background()
{
  return VPI_ISystem::create(false, true);
}

VPI_TSuccess VPI_ISystem::dispose()
{
  if (g_vpi_system) {
    delete g_vpi_system;
    g_vpi_system = nullptr;
    return VPI_kSuccess;
  }
  return VPI_kFailure;
}

VPI_TSuccess VPI_ISystem::dispose_background()
{
  if (g_vpi_system_background) {
    delete g_vpi_system_background;
    g_vpi_system_background = nullptr;
    return VPI_kSuccess;
  }
  return VPI_kFailure;
}

VPI_ISystem *VPI_ISystem::get()
{
  return g_vpi_system;
}

VPI_ISystem *VPI_ISystem::get_background()
{
  return g_vpi_system_background;
}

VPI_System::VPI_System() : qt_window_(nullptr), qt_app_(nullptr)
{
  start_time_ = 0;
}

VPI_System::~VPI_System() = default;

VPI_Window *VPI_System::create_window(char const *title,
                                      int32_t left,
                                      int32_t top,
                                      uint32_t width,
                                      uint32_t height,
                                      VPI_Window const *parent_window) noexcept
{
  qt_window_->create_window(title, left, top, width, height, parent_window);
  qt_window_->window_manager_->add_window(qt_window_);
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
  qt_app_->installEventFilter(this);

  clog::clog_init("VPI", "vpi_events.log", "VPI System Initialized");

  qt_window_ = new VPI_QtWindow();
  timer_.start();
  // std::chrono::duration_cast<std::chrono::milliseconds>(
  //       std::chrono::steady_clock::now().time_since_epoch()).count();
  start_time_ = timer_.elapsed();

  return VPI_kSuccess;
}

VPI_TSuccess VPI_System::exit(bool &is_running)
{
  qt_app_->removeEventFilter(this);
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

VPI_TSuccess VPI_System::register_window(VPI_Window *window) noexcept
{
  return qt_window_->window_manager_->add_window(window);
}

VPI_Window *VPI_System::get_window_under_cursor(int32_t x, int32_t y) const noexcept
{
  std::vector<VPI_Window *> windows = qt_window_->window_manager_->get_windows();
  std::vector<VPI_Window *>::reverse_iterator iwindow_iter;

  for (iwindow_iter = windows.rbegin(); iwindow_iter != windows.rend(); ++iwindow_iter) {
    VPI_Window *win = *iwindow_iter;

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

VPI_TSuccess VPI_System::process_events_impl(bool wait_for_event)
{
  if (wait_for_event) {
    QApplication::exec();
  }
  else {
    QApplication::processEvents();
  }

  return VPI_kSuccess;
}

bool VPI_System::event_filter(QObject *obj, QEvent *event)
{
  VPI_QtWindow *window = nullptr;

  if (obj->isWidgetType()) {
    QWidget *widget = qobject_cast<QWidget *>(obj);
    printf("Filter hit for widget: %p, event: %d\n", (void *)obj, event->type());
    while (widget) {
      window = qobject_cast<VPI_QtWindow *>(widget);
      if (window) {
        break;
      }
      widget = widget->parentWidget();
    }
  }

  if (!window) {
    return false;
  }

  switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      auto *ke = dynamic_cast<QKeyEvent *>(event);
      VPI_TEventKeyData data = {};
      data.key = ke->key();
      data.modifiers = ke->modifiers();
      data.is_repeat = ke->isAutoRepeat();
      (void)window->event_manager_->push_event(std::make_unique<VPI_KeyEvent>(window, data));
      break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick: {
      auto *me = dynamic_cast<QMouseEvent *>(event);
      VPI_TEventMouseButtonData data = {};
      data.x = static_cast<int32_t>(me->position().x());
      data.y = static_cast<int32_t>(me->position().y());

      if (me->button() == Qt::LeftButton) {
        data.button = vektor::LEFTMOUSE;
      }
      else if (me->button() == Qt::RightButton) {
        data.button = vektor::RIGHTMOUSE;
      }
      else if (me->button() == Qt::MiddleButton) {
        data.button = vektor::MIDDLEMOUSE;
      }
      else {
        data.button = static_cast<uint32_t>(me->button());
      }

      data.modifiers = me->modifiers();
      (void)window->event_manager_->push_event(std::make_unique<VPI_MouseEvent>(window, data));
      break;
    }
    case QEvent::MouseMove: {
      auto *me = dynamic_cast<QMouseEvent *>(event);
      VPI_TEventCursorData data = {};
      data.x = static_cast<int32_t>(me->position().x());
      data.y = static_cast<int32_t>(me->position().y());
      (void)window->event_manager_->push_event(std::make_unique<VPI_MouseMoveEvent>(window, data));
      break;
    }
    case QEvent::Wheel: {
      auto *we = dynamic_cast<QWheelEvent *>(event);
      VPI_TEventMouseWheelData data = {};
      data.x = static_cast<int32_t>(we->position().x());
      data.y = static_cast<int32_t>(we->position().y());
      data.delta_x = we->angleDelta().x();
      data.delta_y = we->angleDelta().y();
      data.modifiers = we->modifiers();
      (void)window->event_manager_->push_event(
          std::make_unique<VPI_MouseWheelEvent>(window, data));
      break;
    }
    case QEvent::Resize: {
      (void)window->event_manager_->push_event(
          std::make_unique<VPI_WindowEvent>(VPI_kWindowResize, window));
      break;
    }
    case QEvent::Move: {
      (void)window->event_manager_->push_event(
          std::make_unique<VPI_WindowEvent>(VPI_kWindowMove, window));
      break;
    }
    case QEvent::Close: {
      (void)window->event_manager_->push_event(
          std::make_unique<VPI_WindowEvent>(VPI_kWindowClose, window));
      break;
    }
    default:
      break;
  }

  return false;
}
}  // namespace vpi
