#include "UI_style_manager.h"
#include "../../intern/config/CONFIG_manager.h"

namespace qt::ui {
QString UI_style_manager::get_qt_style()
{
  auto &cfg = config::ConfigManager::instance();
  // Load the stylesheet config
  cfg.load("qt_style", "/Users/lazycodebaker/Documents/Dev/CPP/vektor/intern/config/ini/qt_style.ini");

  // Fetch styles with fallbacks mapping to the original defaults
  QString main_bg = QString::fromStdString(
      cfg.get_string("qt_style", "MainWindow", "BackgroundColor", "#3d3d3d"));
  QString main_fg = QString::fromStdString(
      cfg.get_string("qt_style", "MainWindow", "TextColor", "#e0e0e0"));
  QString main_font = QString::fromStdString(
      cfg.get_string("qt_style", "MainWindow", "FontFamily", "'Inter', 'Segoe UI', sans-serif"));
  QString main_font_size = QString::fromStdString(
      cfg.get_string("qt_style", "MainWindow", "FontSize", "10px"));

  // Menu bar
  QString menu_bg = QString::fromStdString(
      cfg.get_string("qt_style", "MenuBar", "BackgroundColor", "#333333"));
  QString menu_border = QString::fromStdString(
      cfg.get_string("qt_style", "MenuBar", "BorderBottomColor", "#2b2b2b"));
  QString menu_sel_bg = QString::fromStdString(
      cfg.get_string("qt_style", "MenuBar", "ItemSelectedBackgroundColor", "#4d4d4d"));

  // Tree view
  QString tree_bg = QString::fromStdString(
      cfg.get_string("qt_style", "TreeView", "BackgroundColor", "#212121"));
  QString tree_alt_bg = QString::fromStdString(
      cfg.get_string("qt_style", "TreeView", "AlternateBackgroundColor", "#292929"));
  QString tree_sel_bg = QString::fromStdString(
      cfg.get_string("qt_style", "TreeView", "ItemSelectedBackgroundColor", "#4c6a8d"));
  QString tree_hover_bg = QString::fromStdString(
      cfg.get_string("qt_style", "TreeView", "ItemHoverBackgroundColor", "#333"));

  // Button
  QString btn_bg = QString::fromStdString(
      cfg.get_string("qt_style", "PushButton", "BackgroundColor", "#555555"));
  QString btn_border = QString::fromStdString(
      cfg.get_string("qt_style", "PushButton", "BorderColor", "#444"));
  QString btn_hover_bg = QString::fromStdString(
      cfg.get_string("qt_style", "PushButton", "HoverBackgroundColor", "#666666"));
  QString btn_hover_border = QString::fromStdString(
      cfg.get_string("qt_style", "PushButton", "HoverBorderColor", "#777"));
  QString btn_pressed_bg = QString::fromStdString(
      cfg.get_string("qt_style", "PushButton", "PressedBackgroundColor", "#cf7c06"));

  return QString(R"(
            /* Main Application background */
            QMainWindow {
                background-color: %1;
                color: %2;
                font-family: %3;
                font-size: %4;
            }
            
            /* Menu bar top level style */
            QMenuBar {
                background-color: %5;
                color: %2;
                border-bottom: 1px solid %6;
                padding: 2px;
            }
            QMenuBar::item {
                padding: 4px 8px;
            }
            QMenuBar::item:selected {
                background-color: %7;
            }

            /* Toolbars (Left-aligned typically) */
            QToolBar {
                background-color: %5;
                border-right: 1px solid %6;
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
                background-color: %6;
                padding: 4px 8px;
                font-weight: bold;
                color: #999;
            }

            /* Tree views (Outliner and Hierarchy) */
            QTreeView {
                background-color: %8;
                alternate-background-color: %9;
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
                background-color: %10;
                color: white;
            }
            QTreeView::item:hover {
                background-color: %11;
            }
            QTreeView::branch:has-children:!has-siblings:closed,
            QTreeView::branch:closed:has-children:has-siblings {
                border-image: none;
                image: none;
            }

            /* Standard Buttons */
            QPushButton {
                background-color: %12;
                color: white;
                border: 1px solid %13;
                border-radius: 2px;
                padding: 2px 6px;
                font-size: 11px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: %14;
                border-color: %15;
            }
            QPushButton:pressed {
                background-color: %16;
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
                border: 1px solid %15;
                width: 8px;
                height: 8px;
                margin: -3px 0;
            }

            /* Scrollbars */
            QScrollBar:vertical {
                border: none;
                background: %6;
                width: 6px;
            }
            QScrollBar::handle:vertical {
                background: %13;
                min-height: 20px;
                margin: 1px;
                border-radius: 2px;
            }

            /* Tab Bar (Bottom and Center area) */
            QTabWidget::pane {
                border: 1px solid %6;
                background-color: %1;
            }
            QTabBar::tab {
                background-color: %5;
                color: #999;
                padding: 2px 10px;
                font-size: 11px;
            }
            QTabBar::tab:selected {
                background-color: %1;
                color: white;
                border-bottom: 2px solid %16;
            }

            /* Input controls */
            QLineEdit {
                background-color: #181818;
                border: 1px solid #111;
                border-radius: 3px;
                color: #eee;
                padding: 1px 4px;
                font-size: 11px;
                selection-background-color: %10;
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
                border-top: 1px solid %6;
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
        )")
      .arg(main_bg)
      .arg(main_fg)
      .arg(main_font)
      .arg(main_font_size)
      .arg(menu_bg)
      .arg(menu_border)
      .arg(menu_sel_bg)
      .arg(tree_bg)
      .arg(tree_alt_bg)
      .arg(tree_sel_bg)
      .arg(tree_hover_bg)
      .arg(btn_bg)
      .arg(btn_border)
      .arg(btn_hover_bg)
      .arg(btn_hover_border)
      .arg(btn_pressed_bg);
}
}  // namespace qt::ui
