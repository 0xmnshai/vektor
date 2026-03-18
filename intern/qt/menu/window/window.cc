#include <QAction>
#include <QDockWidget>
#include <QGuiApplication>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QScreen>

#include "../../../vpi/intern/VPI_QtWindow.hh"

#include "window.hh"

namespace qt::menu {

void MENU_window_register(void *window)
{
  auto *vpi_window = static_cast<vpi::VPI_QtWindow *>(window);
  QMenuBar *menu_bar = vpi_window->menuBar();

  QMenu *menu = menu_bar->addMenu("Window");

  QAction *new_window_action = menu->addAction("New Window");
  QObject::connect(new_window_action, &QAction::triggered, [vpi_window]() {
    auto *new_window = new vpi::VPI_QtWindow();

    // Center the new window on the primary screen
    // we can add this in vkScreen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
      QRect screen_geometry = screen->geometry();
      int width = 620;
      int height = 440;
      int x = screen_geometry.left() + (screen_geometry.width() - width) / 2;
      int y = screen_geometry.top() + (screen_geometry.height() - height) / 2;
      new_window->setGeometry(x, y, width, height);
    }

    new_window->setup_menus();
    QDockWidget *viewport_dock = new_window->setup_new_window_with_viewport();
    // viewport_dock->setFloating(true);
    // viewport_dock->setWindowTitle("Viewport");
    // viewport_dock->setFeatures(QDockWidget::DockWidgetMovable |
    // QDockWidget::DockWidgetFloatable); viewport_dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    // vpi::VPI_QtWindow *viewport_window = vpi::VPI_System::get()->wrap_widget(viewport_dock);

    // // Register the new window in the window manager list
    vpi::VPI_WindowManager &wm = vpi_window->get_window_manager();
    wm.add_window(new_window);

    new_window->show();
  });
  menu->addSeparator();

  QAction *fullscreen_action = menu->addAction("Toggle Window Fullscreen");
  fullscreen_action->setCheckable(true);
  fullscreen_action->setChecked(false);
  fullscreen_action->setShortcut(QKeySequence("F11"));

  QObject::connect(fullscreen_action, &QAction::triggered, [vpi_window]() {
    vpi_window->setWindowState(vpi_window->windowState() ^ Qt::WindowFullScreen);
  });
}

}  // namespace qt::menu
