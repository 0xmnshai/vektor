#include <cstdlib>

#include "creator.h"
#include "intern/CLG_init.hh"
#include "lib/intern/context.hh"
#include "windowmanager/intern/wm_init_exit.hh"

const char *id = "main";
const char *file_name = "editor.log";
const char *var = "Editor";

vektor::lib::vkContext vkC = {};

int main(int argc, const char **argv)
{
  clog::clog_init(id, file_name, var);

  vektor::runtime::main_args_parse(argc, argv);
  
  vektor::editor::WM_init(&vkC, argc, argv);

  clog::clg_exit();

  return EXIT_SUCCESS;
}
