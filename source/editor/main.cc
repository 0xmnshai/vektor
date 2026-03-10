#include <filesystem>
#include <cstdlib>

#include "../../intern/clog/CLG_log.h"
#include "creator.h"
#include "creator_global.h"
#include "intern/VPI_System.hh"
#include "vektor_version.h"

int main(int argc, const char **argv)
{
  clog::CLG_init();
  clog::CLG_level_set(clog::CLG_LEVEL_INFO);
  clog::CLG_output_use_timestamp_set(1);

  std::filesystem::path log_dir = std::filesystem::path(VEKTOR_SOURCE_DIR) / "logs";
  std::filesystem::create_directories(log_dir);
  std::filesystem::path log_path = log_dir / "editor.log";

  FILE *log_file = fopen(log_path.c_str(), "a");
  if (log_file) {
    clog::CLG_output_extra_set(log_file);
  }

  CLG_LOGREF_DECLARE_GLOBAL(V_LOG, "main");
  CLOG_INFO(V_LOG, "Editor Starting");

  vpi::VPI_System system;
  system.init();

  vektor::runtime::main_args_parse(argc, argv);

  vpi::VPI_IWindow *editor_window = nullptr;

  if (!vektor::creator::G.background) {
    editor_window = system.create_window("Vektor Editor", 0, 0, 1280, 720, nullptr);
  }

  vektor::runtime::initialize(&system, editor_window);

  bool is_running = true;

  while (is_running) {
    if (editor_window) {
      is_running = editor_window->process_events(false);
    }
    vektor::runtime::tick();
    // ui render
  }

  // vektor::runtime::shutdown();
  system.exit(is_running);

  clog::CLG_exit();

  return EXIT_SUCCESS;
}
