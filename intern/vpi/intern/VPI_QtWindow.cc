#include <QDockWidget>

#include "VPI_EventManager.hh"
#include "VPI_QtWindow.hh"
#include "VPI_WindowManager.hh"

#include "../../intern/qt/dock/common/WIDGET_area.h"
#include "../../intern/qt/dock/scene/SCN_setup.h"

namespace vpi {

VPI_QtWindow::VPI_QtWindow()
    : gl_widget_(nullptr),
      window_manager_(new VPI_WindowManager()),
      event_manager_(new VPI_EventManager()),
      style_manager_(new qt::ui::UI_style_manager()),
      widget_(nullptr)
{
}

VPI_QtWindow::VPI_QtWindow(QWidget *widget)
    : gl_widget_(nullptr),
      window_manager_(new VPI_WindowManager()),
      event_manager_(new VPI_EventManager()),
      style_manager_(new qt::ui::UI_style_manager()),
      widget_(widget)
{
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

  setup_menus();
  setup_docks();

  qt::scene::SCN_init_default_scene();

  show();
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
  bounds.set(r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
}

void VPI_QtWindow::get_client_bounds(VPI_Rect &bounds) const
{
  QRect r = geometry();
  bounds.set(r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
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

void VPI_QtWindow::setup_docks()
{
  setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AllowNestedDocks |
                 QMainWindow::AnimatedDocks | QMainWindow::GroupedDragging);

  // Configure corners to match the layout in the screenshot (Bottom area spans width)
  setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

  // Default features for all docks
  QDockWidget::DockWidgetFeatures features = QDockWidget::DockWidgetMovable |
                                             QDockWidget::DockWidgetFloatable |
                                             QDockWidget::DockWidgetClosable;

  // --- SCENE DOCK (The Viewport) ---
  auto *scene_dock = new QDockWidget("Scene", this);
  scene_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
  scene_dock->setFeatures(features);
  auto *view_port_area = new qt::dock::AreaWidget(qt::dock::EditorType::VIEWPORT_3D, this);
  scene_dock->setWidget(view_port_area);
  addDockWidget(Qt::LeftDockWidgetArea, scene_dock);

  // --- OUTLINER DOCK ---
  auto *outliner_dock = new QDockWidget("Outliner", this);
  outliner_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
  outliner_dock->setFeatures(features);
  outliner_dock->setMinimumWidth(200);
  auto *outliner_area = new qt::dock::AreaWidget(qt::dock::EditorType::OUTLINER, this);
  outliner_dock->setWidget(outliner_area);
  addDockWidget(Qt::RightDockWidgetArea, outliner_dock);

  // --- PROPERTIES DOCK ---
  auto *properties_dock = new QDockWidget("Properties", this);
  properties_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
  properties_dock->setFeatures(features);
  properties_dock->setMinimumWidth(200);
  auto *properties_area = new qt::dock::AreaWidget(qt::dock::EditorType::PROPERTIES, this);
  properties_dock->setWidget(properties_area);
  addDockWidget(Qt::RightDockWidgetArea, properties_dock);

  // Stack Properties below Outliner on the right
  splitDockWidget(outliner_dock, properties_dock, Qt::Vertical);

  // Set a central widget to provide a stable reference for docking
  // setCentralWidget(new QWidget(this));
}
}  // namespace vpi
