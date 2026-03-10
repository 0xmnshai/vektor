#include <iostream>

#include "VPI_IContext.h"
#include "creator.h"
#include "creator_args.hh"

namespace vektor::runtime {
vpi::VPI_ISystem *g_system = nullptr;
vpi::VPI_IWindow *g_main_window = nullptr;
vpi::VPI_IContext *g_graphics_context = nullptr;

void main_args_parse(int argc, const char **argv)
{
  vektor::creator::main_args_handle(argc, argv);
}

void initialize(vpi::VPI_ISystem *sys, vpi::VPI_IWindow *window)
{
  std::cout << "initialize from runtime creator.cc" << std::endl;
  g_system = sys;
  g_main_window = window;

  // The Runtime creates the OpenGL/Metal Context and attaches it
  // to the window provided by the Editor.
  // g_graphics_context = new vpi::VPI_ContextGL();  // Or MTL

  // Grab the native OS handle (HWND / NSWindow) from the generic window
  void const *native_handle = nullptr;
  // g_main_window->get_native_handle(&native_handle);

  // Now the runtime context knows where to hook into
  // g_graphics_context->create(native_handle);
}

void tick()
{
  if (g_main_window) {
    int32_t x, y;
    (void)g_main_window->get_cursor_position(&x, &y);

    vpi::VPI_IWindow *win_under_cursor = g_system->get_window_under_cursor(x, y);
    if (win_under_cursor) {
      char const *title = nullptr;
      (void)win_under_cursor->get_title(&title);
      if (title) {
        std::cout << "Window under cursor: " << title << std::endl;
      }
    }
  }

  // std::cout << "tick from runtime creator.cc" << std::endl;
}
}  // namespace vektor::runtime

// test for commit
