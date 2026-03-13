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
};
}  // namespace qt::ui
