#include <cstdlib>
#include <iostream>

#include "creator.h"
#include "creator_global.h"
#include "intern/VPI_System.hh"

int main(int argc, const char **argv)
{
  std::cout << "Editor" << std::endl;

  vpi::VPI_System system;
  system.init();

  vektor::runtime::main_args_parse(argc, argv);

  vpi::VPI_IWindow *editor_window = nullptr;

  if (!vektor::creator::G.background) {
    editor_window = system.create_window(
        "Vektor Editor", 0, 0, 1280, 720, nullptr);
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

  return EXIT_SUCCESS;
}
