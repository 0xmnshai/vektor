
#include <cstdlib>
#include <cstring>

#include <csignal>
#include <iostream>

#include "creator_args.h"
#include "creator_global.h"
#include "creator_intern.h"

namespace vektor {
namespace creator {
ApplicationState app_state = []() {
  ApplicationState app_state{};
  app_state.signal.use_crash_handler = true;
  app_state.signal.use_abort_handler = true;
  app_state.exit_code_on_error.python = 0;
  app_state.main_arg_deferred = nullptr;
  return app_state;
}();
} // namespace creator
} // namespace vektor

int main(int argc, const char **argv) {
  vektor::creator::G.is_break = false;
  vektor::creator::G.background = false;

  vektor::creator::Args args;
  vektor::creator::main_args_setup(args);
  int result = vektor::creator::main_args_handle(argc, argv);
  if (result != 0)
    return result;

  if (vektor::creator::G.project_file) {
    std::cout << "Loading project: " << vektor::creator::G.project_file
              << std::endl;

    std::cout << "Engine initialized." << std::endl;
    std::cout << "Running loop..." << std::endl;

    for (int i = 0; i < 3; ++i) {
      std::cout << "Frame " << i << " processed." << std::endl;
    }
    std::cout << "Engine shut down." << std::endl;
  } else {
    std::cout
        << "No project file specified. Use 'vektor [path/to/project.vproj]'"
        << std::endl;
  }
  return 0;
}