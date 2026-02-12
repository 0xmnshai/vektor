
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "creator_args.h"
#include "creator_global.h"

namespace vektor {
namespace creator {

void Args::add(const std::string &short_arg, const std::string &long_arg,
               const std::string &doc, ArgCallback callback, void *data) {
  args.push_back({short_arg, long_arg, doc, callback, data});
}

void Args::print_help() {
  std::cout << "Vektor Engine\n";
  std::cout << "Usage: vektor [args ...]\n\n";

  for (const auto &arg : args) {
    std::cout << "  ";
    if (!arg.short_arg.empty()) {
      std::cout << arg.short_arg;
      if (!arg.long_arg.empty()) {
        std::cout << ", ";
      }
    }
    if (!arg.long_arg.empty()) {
      std::cout << arg.long_arg;
    }


    size_t len = arg.short_arg.length() + (arg.short_arg.empty() ? 0 : 2) +
                 arg.long_arg.length();
    if (len < 30) {
      std::cout << std::string(30 - len, ' ');
    } else {
      std::cout << "\n" << std::string(32, ' ');
    }

    std::cout << arg.doc << "\n";
  }
  std::cout << "\n";
}

int Args::parse(int argc, const char **argv) {
  for (int i = 1; i < argc; ++i) {
    bool handled = false;
    for (const auto &arg : args) {
      if ((!arg.short_arg.empty() &&
           std::strcmp(argv[i], arg.short_arg.c_str()) == 0) ||
          (!arg.long_arg.empty() &&
           std::strcmp(argv[i], arg.long_arg.c_str()) == 0)) {

        int consumed = arg.callback(argc - i, argv + i, arg.data);
        if (consumed < 0) {

          return consumed;
        }
        i += consumed;
        handled = true;
        break;
      }
    }

    if (!handled) {
      if (argv[i][0] != '-') {

        G.project_file = argv[i];
      } else {
        std::cerr << "Unknown argument: " << argv[i] << "\n";

        exit(1);
      }
    }
  }
  return 0;
}

static int arg_handle_print_help(int  , const char **  ,
                                 void *data) {
  Args *args = static_cast<Args *>(data);
  args->print_help();
  exit(0);
  return 0;
}

static int arg_handle_version(int  , const char **  ,
                              void *  ) {

  std::cout << "Vektor Engine 0.0.1 (Alpha)\n";
  exit(0);
  return 0;
}

static int arg_handle_background_mode(int  , const char **  ,
                                      void *  ) {
  std::cout << "Running in background mode (headless).\n";

  return 0;
}

static int arg_handle_window_geometry(int argc, const char **argv,
                                      void *  ) {
  if (argc < 5) {
    std::cerr << "Error: --window-geometry requires 4 arguments (x y w h)\n";
    exit(1);
  }
  int x = std::atoi(argv[1]);
  int y = std::atoi(argv[2]);
  int w = std::atoi(argv[3]);
  int h = std::atoi(argv[4]);

  std::cout << "Window Geometry: " << x << ", " << y << ", " << w << "x" << h
            << "\n";

  return 4;




  return 4;
}

static int arg_handle_log_level(int argc, const char **argv, void *  ) {
  if (argc < 2) {
    std::cerr << "Error: --log-level requires an argument\n";
    exit(1);
  }
  std::cout << "Log Level: " << argv[1] << "\n";

  return 1;
}

static int arg_handle_debug(int  , const char **  ,
                            void *  ) {
  std::cout << "Debug mode enabled.\n";

  return 0;
}






void main_args_setup(Args &args) {
  args.add("-h", "--help", "Print this help text and exit",
           arg_handle_print_help, &args);
  args.add("-v", "--version", "Print Vektor version and exit",
           arg_handle_version);


  args.add("-b", "--background", "Run in background (headless) mode",
           arg_handle_background_mode);
  args.add("-p", "--window-geometry", "<x> <y> <w> <h> Open with geometry",
           arg_handle_window_geometry);


  args.add("", "--log-level", "<level> Set logging verbosity",
           arg_handle_log_level);
  args.add("-d", "--debug", "Turn debugging on", arg_handle_debug);
}

int main_args_handle(int argc, const char **argv) {
  Args args;
  main_args_setup(args);
  return args.parse(argc, argv);
}



}
}