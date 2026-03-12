
#include "creator.h"
#include "../../intern/clog/intern/CLG_init.hh"
#include "CLG_log.h"
#include "VPI_IContext.h"
#include "creator_args.hh"
#include "creator_intern.h"

namespace vektor::runtime {
vpi::VPI_ISystem *g_system = nullptr;
vpi::VPI_IWindow *g_main_window = nullptr;
vpi::VPI_IContext *g_graphics_context = nullptr;

CLG_LOGREF_DECLARE_GLOBAL(V_LOG, "creator");

creator::ApplicationState app_state = []() {
  creator::ApplicationState app_state{};
  app_state.signal.use_crash_handler = true;
  app_state.signal.use_abort_handler = true;
  app_state.exit_code_on_error.python = 0;
  return app_state;
}();

void main_args_parse(int argc, const char **argv)
{
  vektor::creator::main_args_handle(argc, argv);
}

void initialize(vpi::VPI_ISystem *sys, vpi::VPI_IWindow *window)
{
  clog::clog_init("creator", "runtime.log", "runtime");

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
        CLOG_INFO(V_LOG, "Window under cursor: %s", title);
      }
    }
  }
}
}  // namespace vektor::runtime

// test for commit
