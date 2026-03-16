#pragma once

namespace qt::menu {

// Menu Register
void window_menu_setup(void /** VPI_QtWindow */ *window);

// Register menu items
void MENU_file_register(void /** VPI_QtWindow */ *window);
void MENU_window_register(void /** VPI_QtWindow */ *window);
void MENU_edit_register(void /** VPI_QtWindow */ *window);
void MENU_help_register(void /** VPI_QtWindow */ *window);
}  // namespace qt::menu
