#include <iostream>

#include "VPI_IContext.h"
#include "creator.h"

namespace vektor::runtime {
vpi::VPI_ISystem *g_system = nullptr;
vpi::VPI_IWindow *g_main_window = nullptr;
vpi::VPI_IContext *g_graphics_context = nullptr;

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
  //         // Run physics, gameplay scripts, etc.

  std::cout << "tick from runtime creator.cc" << std::endl;

  //         // Render the scene using g_graphics_context
}
}  // namespace vektor::runtime


// test for commit 