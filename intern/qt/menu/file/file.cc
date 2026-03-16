#include <QCoreApplication>
#include <QMenu>
#include <QMenuBar>

#include "../../../vpi/intern/VPI_QtWindow.hh"
#include "file.hh"

namespace qt::menu {

void MENU_file_register(void *window)
{
  QMenuBar *menu_bar = static_cast<vpi::VPI_QtWindow *>(window)->menuBar();

  QMenu *menu = menu_bar->addMenu("File");
  menu->addAction("New");
  menu->addAction("Open");
  menu->addAction("Save");
  menu->addAction("Save As");

  menu->addSeparator();

  QAction *quit_action = menu->addAction("Quit");
  quit_action->setShortcut(QKeySequence::Quit);
  quit_action->setMenuRole(QAction::QuitRole);

  QObject::connect(quit_action, &QAction::triggered, []() {
    qApp->quit();
  });
}

}  // namespace qt::menu
