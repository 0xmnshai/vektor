#include <QApplication>
#include <cstdio>

#include "../../../../intern/clog/CLG_log.h"
#include "../../../../intern/vpi/VPI_ISystem.h"
#include "../../../runtime/creator.h"
#include "../../../runtime/creator_global.h"
#include "../../intern/vpi/VPI_IWindow.h"
#include "vektor_version.h"
#include "wm_init_exit.hh"

namespace vektor::editor {
CLG_LOGREF_DECLARE_GLOBAL(EDITOR_LOG, "editor");

void WM_init(lib::vkContext *vkC, int argc, const char **argv)
{
  vpi::VPI_IWindow *editor_window = nullptr;

  if (vpi::VPI_ISystem::create() != VPI_kSuccess) {
    CLOG_ERROR(EDITOR_LOG, "Failed to create VPI system");
    return;
  }

  vpi::VPI_ISystem *system = vpi::VPI_ISystem::get();
  system->init();

  auto screen_size = QApplication::screens()[0]->size();

  if (!vektor::creator::G.background) {
    char title_buf[256];
    if (vektor::creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
      snprintf(
          title_buf, sizeof(title_buf), "Vektor Editor ( version : %s ) - OpenGL", VEKTOR_VERSION);
    }
    else if (vektor::creator::G.gpu_backend == creator::GPU_BACKEND_METAL) {
      snprintf(
          title_buf, sizeof(title_buf), "Vektor Editor ( version : %s ) - Metal", VEKTOR_VERSION);
    }
    editor_window = system->create_window(
        title_buf, 0, 0, screen_size.width(), screen_size.height(), nullptr);
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
