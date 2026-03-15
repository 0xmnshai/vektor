#include "wm_init_exit.hh"
#include "../../../../intern/clog/CLG_log.h"
#include "../../../../intern/vpi/VPI_ISystem.h"
#include "../../../runtime/creator.h"
#include "../../../runtime/creator_global.h"
#include "../../intern/vpi/VPI_IWindow.h"

namespace vektor::editor {
CLG_LOGREF_DECLARE_GLOBAL(V_LOG, "editor");

void WM_init(lib::vkContext *vkC, int argc, const char **argv)
{
  vpi::VPI_IWindow *editor_window = nullptr;

  if (vpi::VPI_ISystem::create() != VPI_kSuccess) {
    CLOG_ERROR(V_LOG, "Failed to create VPI system");
    return;
  }

  vpi::VPI_ISystem *system = vpi::VPI_ISystem::get();
  system->init();

  if (!vektor::creator::G.background) {
    editor_window = system->create_window("Vektor Editor", 0, 0, 1280, 720, nullptr);
  }

  vektor::runtime::initialize(system, editor_window);

  bool is_running = true;

  while (is_running) {
    if (editor_window) {
      is_running = editor_window->process_events(false);
    }
    vektor::runtime::tick();
    // ui render
  }

  // vektor::runtime::shutdown();
  system->exit(is_running);
}

void WM_exit()
{
  bool is_running = false;
  vpi::VPI_ISystem::get()->exit(is_running);
}
}  // namespace vektor::editor
