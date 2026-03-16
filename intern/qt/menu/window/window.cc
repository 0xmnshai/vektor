#include <QMenu>
#include <QMenuBar>
#include <QObject>

#include "../../../vpi/intern/VPI_QtWindow.hh"
#include "window.hh"

namespace qt::menu {

void MENU_window_register(void *window)
{
  auto *vpi_window = static_cast<vpi::VPI_QtWindow *>(window);
  QMenuBar *menu_bar = vpi_window->menuBar();

  QMenu *menu = menu_bar->addMenu("Window");

  menu->addAction("New Window");
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
