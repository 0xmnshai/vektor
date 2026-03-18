
#include <QMenuBar>

#include "../MENU_setup.h"

namespace qt::menu {

void window_menu_setup(void *window)
{
  MENU_file_register(window);
  MENU_window_register(window);
  MENU_edit_register(window);
  //   MENU_help_register(menu_bar);
}


/**

help:

Manual
Support
Release Notes
Developer Documentation
Rust API Reference
Python API Reference
*/

}  // namespace qt::menu
