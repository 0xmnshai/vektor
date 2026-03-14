#include <qwidget.h>

#include "../../intern/clog/CLG_log.h"
#include "../../intern/qt/ui/UI_style_manager.h"
#include "../../source/editor/windowmanager/wm_event_types.h"
#include "VPI_System.hh"
#include "intern/CLG_init.hh"
#include "intern/VPI_QtWindow.hh"

namespace vpi {

CLG_LOGREF_DECLARE_GLOBAL(SYS_LOG, "system");

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

VPI_QtWindow *VPI_System::create_window(char const *title,
                                        int32_t left,
                                        int32_t top,
                                        uint32_t width,
                                        uint32_t height,
                                        VPI_QtWindow const *parent_window) noexcept
{
  qt_window_->create_window(title, left, top, width, height, parent_window);
  qt_window_->window_manager_->add_window(qt_window_);
  return qt_window_;
}

VPI_TSuccess VPI_System::init()
{
  static int argc = 1;
  static char *argv[] = {(char *)"vektor", nullptr};

  clog::clog_init("system", "editor.log", "editor");

  QSurfaceFormat format;
  format.setVersion(6, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  QSurfaceFormat::setDefaultFormat(format);

  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

  qt_app_ = new QApplication(argc, argv);
  qt_app_->setStyleSheet(qt::ui::UI_style_manager::get_qt_style());
  qt_app_->installEventFilter(this);

  qt_window_ = new VPI_QtWindow();
  timer_.start();
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

VPI_TSuccess VPI_System::register_window(VPI_QtWindow *window) noexcept
{
  return qt_window_->window_manager_->add_window(window);
}

VPI_QtWindow *VPI_System::wrap_widget(QWidget *widget)
{
  if (widget == nullptr) {
    CLOG_ERROR(SYS_LOG, "Widget is null");
    return nullptr;
  }

  for (auto *win : qt_window_->window_manager_->get_windows()) {
    if (win->get_widget() == widget) {
      return win;
    }
  }

  auto *new_window = new VPI_QtWindow();
  (void)this->register_window(new_window);
  new_window->set_widget(widget);

  if (auto *dock = qobject_cast<QDockWidget *>(widget)) {
    (void)new_window->set_title(dock->windowTitle().toStdString().c_str());
  }

  return new_window;
}

VPI_QtWindow *VPI_System::get_window_under_cursor(int32_t /*x*/, int32_t /*y*/) const noexcept
{
  QWidget *widget = QApplication::widgetAt(QCursor::pos());
  if (!widget) {
    return nullptr;
  }

  while (widget) {
    // Check if it's a VPI_QtWindow (Main Window or already wrapped window)
    if (auto *vpi_win = qobject_cast<VPI_QtWindow *>(widget)) {
      return vpi_win;
    }

    // Check if it's a floating dock widget (Panel)
    if (auto *dock = qobject_cast<QDockWidget *>(widget)) {
      if (dock->isFloating()) {
        // We found a floating panel, wrap it if not already wrapped
        return const_cast<VPI_System *>(this)->wrap_widget(dock);
      }
    }

    widget = widget->parentWidget();
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
      CLOG_INFO(SYS_LOG, "Key press: %d", data.key);
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
      CLOG_INFO(SYS_LOG, "Mouse button press: %d at %d, %d", data.button, data.x, data.y);
      (void)window->event_manager_->push_event(std::make_unique<VPI_MouseEvent>(window, data));
      break;
    }
    case QEvent::MouseMove: {
      auto *me = dynamic_cast<QMouseEvent *>(event);
      VPI_TEventCursorData data = {};
      data.x = static_cast<int32_t>(me->position().x());
      data.y = static_cast<int32_t>(me->position().y());
      CLOG_INFO(SYS_LOG, "Mouse move: %d, %d", data.x, data.y);
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
      CLOG_INFO(
          SYS_LOG, "Mouse Wheel: %d, %d at %d, %d", data.delta_x, data.delta_y, data.x, data.y);
      (void)window->event_manager_->push_event(
          std::make_unique<VPI_MouseWheelEvent>(window, data));
      break;
    }
    case QEvent::Resize: {
      CLOG_INFO(SYS_LOG, "Window Resize Event Occured");
      (void)window->event_manager_->push_event(
          std::make_unique<VPI_WindowEvent>(VPI_kWindowResize, window));
      break;
    }
    case QEvent::Move: {
      CLOG_INFO(SYS_LOG, "Window Move Event Occured");
      (void)window->event_manager_->push_event(
          std::make_unique<VPI_WindowEvent>(VPI_kWindowMove, window));
      break;
    }
    case QEvent::Close: {
      CLOG_INFO(SYS_LOG, "Window Close Event Occured");
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
