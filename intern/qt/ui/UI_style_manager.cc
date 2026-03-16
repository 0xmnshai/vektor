#include "UI_style_manager.h"

namespace qt::ui {
QString UI_style_manager::get_qt_style()
{
  return R"(
            /* Main Application background */
            QMainWindow {
                background-color: #3d3d3d;
                color: #e0e0e0;
                font-family: 'Inter', 'Segoe UI', sans-serif;
                font-size: 10px;
            }
            
            /* Menu bar top level style */
            QMenuBar {
                background-color: #333333;
                color: #e0e0e0;
                border-bottom: 1px solid #2b2b2b;
                padding: 2px;
            }
            QMenuBar::item {
                padding: 4px 8px;
            }
            QMenuBar::item:selected {
                background-color: #4d4d4d;
            }

            /* Toolbars (Left-aligned typically) */
            QToolBar {
                background-color: #333333;
                border-right: 1px solid #2b2b2b;
                spacing: 4px;
                padding: 4px;
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
                padding: 4px 8px;
                font-weight: bold;
                color: #999;
            }

            /* Tree views (Outliner and Hierarchy) */
            QTreeView {
                background-color: #212121;
                alternate-background-color: #292929;
                padding: 0px;
                border: none;
                color: #ccc;
                outline: none;
                font-size: 11px;
            }
            QTreeView::item {
                height: 20px;
                padding: 0px;
            }
            QTreeView::item:selected {
                background-color: #4c6a8d;
                color: white;
            }
            QTreeView::item:hover {
                background-color: #333;
            }
            QTreeView::branch:has-children:!has-siblings:closed,
            QTreeView::branch:closed:has-children:has-siblings {
                border-image: none;
                image: none;
            }

            /* Standard Buttons */
            QPushButton {
                background-color: #555555;
                color: white;
                border: 1px solid #444;
                border-radius: 2px;
                padding: 2px 6px;
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
                font-size: 11px;
            }

            /* Slider (Transform controls) */
            QSlider::groove:horizontal {
                height: 2px;
                background: #1d1d1d;
            }
            QSlider::handle:horizontal {
                background-color: #a0a0a0;
                border: 1px solid #777;
                width: 8px;
                height: 8px;
                margin: -3px 0;
            }

            /* Scrollbars */
            QScrollBar:vertical {
                border: none;
                background: #2b2b2b;
                width: 6px;
            }
            QScrollBar::handle:vertical {
                background: #444;
                min-height: 20px;
                margin: 1px;
                border-radius: 2px;
            }

            /* Tab Bar (Bottom and Center area) */
            QTabWidget::pane {
                border: 1px solid #2b2b2b;
                background-color: #3d3d3d;
            }
            QTabBar::tab {
                background-color: #333333;
                color: #999;
                padding: 2px 10px;
                font-size: 11px;
            }
            QTabBar::tab:selected {
                background-color: #3d3d3d;
                color: white;
                border-bottom: 2px solid #cf7c06;
            }

            /* Input controls */
            QLineEdit {
                background-color: #181818;
                border: 1px solid #111;
                border-radius: 3px;
                color: #eee;
                padding: 1px 4px;
                font-size: 11px;
                selection-background-color: #4c6a8d;
            }
            QDoubleSpinBox {
                background-color: #181818;
                border: 1px solid #111;
                border-radius: 2px;
                color: #ddd;
                padding: 1px 4px;
                font-size: 11px;
            }

            /* Group Boxes (Panel Sections) */
            QGroupBox {
                border: none;
                border-top: 1px solid #2b2b2b;
                border-radius: 0px;
                margin-top: 10px;
                padding-top: 6px;
                font-weight: bold;
                color: #aaa;
                font-size: 11px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0px 4px;
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
};
}  // namespace qt::ui
