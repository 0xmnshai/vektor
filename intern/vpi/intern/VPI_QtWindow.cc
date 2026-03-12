#include "VPI_QtWindow.hh"
#include "intern/VPI_GLWidget.hh"

namespace vpi {

VPI_QtWindow::VPI_QtWindow()
{
  gl_widget_ = new VPI_GLWidget();
  window_manager_ = new VPI_WindowManager();
  event_manager_ = new VPI_EventManager();

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

QString get_qt_style()
{
  return R"(
            /* Main Application background */
            QMainWindow {
                background-color: #3d3d3d;
                color: #e0e0e0;
                font-family: 'Inter', 'Segoe UI', sans-serif;
            }
            
            /* Menu bar top level style */
            QMenuBar {
                background-color: #333333;
                color: #e0e0e0;
                border-bottom: 1px solid #2b2b2b;
            }
            QMenuBar::item:selected {
                background-color: #4d4d4d;
            }

            /* Toolbars (Left-aligned typically) */
            QToolBar {
                background-color: #333333;
                border-right: 1px solid #2b2b2b;
                spacing: 10px;
                padding: 8px;
            }

            /* Dockable panel frames */
            QDockWidget {
                border: none;
                titlebar-close-icon: url(close.png);
                titlebar-normal-icon: url(undock.png);
            }
            QDockWidget::title {
                text-align: left;
                background-color: #2b2b2b;
                padding: 6px 12px;
            }

            /* Tree views (Outliner and Hierarchy) */
            QTreeView {
                background-color: #2b2b2b;
                alternate-background-color: #2f2f2f;
                padding: 5px;
                border: none;
            }
            QTreeView::item {
                height: 24px;
            }
            QTreeView::item:selected {
                background-color: #4c6a8d;
            }

            /* Standard Buttons */
            QPushButton {
                background-color: #555555;
                color: white;
                border: 1px solid #444;
                border-radius: 0px;
                padding: 6px 12px;
                font-size: 11px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: #666666;
                border-color: #777;
            }
            QPushButton:pressed {
                background-color: #cf7c06; /*  Orange */
            }

            /* Label styling */
            QLabel {
                color: #b0b0b0;
                font-weight: 500;
            }

            /* Slider (Transform controls) */
            QSlider::groove:horizontal {
                height: 3px;
                background: #1d1d1d;
            }
            QSlider::handle:horizontal {
                background-color: #a0a0a0;
                border: 1px solid #777;
                width: 12px;
                height: 12px;
                margin: -5px 0;
            }

            /* Scrollbars */
            QScrollBar:vertical {
                border: none;
                background: #2b2b2b;
                width: 12px;
            }
            QScrollBar::handle:vertical {
                background: #444;
                min-height: 20px;
                margin: 2px;
            }

            /* Tab Bar (Bottom and Center area) */
            QTabWidget::pane {
                border: 1px solid #2b2b2b;
                background-color: #3d3d3d;
            }
            QTabBar::tab {
                background-color: #333333;
                color: #999;
                padding: 8px 16px;
                border-radius: 0px;
            }
            QTabBar::tab:selected {
                background-color: #3d3d3d;
                color: white;
                border-bottom: 2px solid #cf7c06;
            }

            /* Input controls */
            QLineEdit {
                border-radius: 0px;
            }
            QDoubleSpinBox {
                background-color: #1d1d1d;
                border: 1px solid #222;
                border-radius: 0px;
                color: #ddd;
                padding: 2px;
            }

            /* Group Boxes (Panel Sections) */
            QGroupBox {
                border: none;
                border-top: 1px solid #2b2b2b;
                border-radius: 0px;
                margin-top: 18px;
                padding-top: 10px;
                font-weight: bold;
                color: #aaa;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 2px 4px;
                left: 0px;
                background-color: transparent;
                color: #ccc;
            }

            /* Specialist IDs */
            QLabel#AxisLabel {
                font-size: 10px;
                font-weight: bold;
                padding: 0 2px;
            }
        )";
}

}  // namespace vpi
