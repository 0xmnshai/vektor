#include <iostream>
#include <utility>

#include "creator_args.hh"
#include "creator_global.h"
#include "vektor_version.h"

namespace vektor::creator {
void Args::add(
    const char *short_arg, const char *long_arg, const char *doc, ArgCallback callback, void *data)
{
  args.push_back(ArgDef{short_arg, long_arg, doc, std::move(callback), data});
}

void Args::print_help()
{
  std::cout << "Vektor Engine Runtime\n";
  std::cout << "Usage: vektor runtime [args ...]\n\n";

  for (const auto &arg : args) {
    std::cout << "  ";
    if (arg.short_arg) {
      std::cout << arg.short_arg;
      if (arg.long_arg) {
        std::cout << ", ";
      }
    }
    if (arg.long_arg) {
      std::cout << arg.long_arg;
    }

    size_t len = strlen(arg.short_arg) + (arg.short_arg ? 0 : 2) + strlen(arg.long_arg);
    if (len < 30) {
      std::cout << std::string(30 - len, ' ');
    }
    else {
      std::cout << "\n" << std::string(32, ' ');
    }

    std::cout << arg.doc << "\n";
  }
  std::cout << "\n";
}

int Args::parse(int argc, const char **argv)
{
  for (int i = 1; i < argc; ++i) {
    bool handled = false;
    for (const auto &arg : args) {
      if ((arg.short_arg && std::strcmp(argv[i], arg.short_arg) == 0) ||
          (arg.long_arg && std::strcmp(argv[i], arg.long_arg) == 0))
      {
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
      }
      else {
        std::cerr << "Unknown argument: " << argv[i] << "\n";
        exit(1);
      }
    }
  }
  return 0;
}

static int arg_handle_print_help(int, const char **, void *data)
{
  Args *args = static_cast<Args *>(data);
  args->print_help();
  exit(0);
  return 0;
}

static int arg_handle_version(int, const char **, void *)
{
  std::cout << "Vektor Engine Runtime \n";
  std::cout << "Version: " << VEKTOR_VERSION << "\n";
  std::cout << "Build Date: " << __DATE__ << "\n";
  std::cout << "Build Time: " << __TIME__ << "\n";
  exit(0);
  return 0;
}

static int arg_handle_background_mode(int, const char **, void *)
{
  std::cout << "Running in background mode (headless).\n";
  G.background = true;
  return 0;
}

void main_args_setup(Args &args)
{
  args.add("-h", "--help", "Print this help text and exit", arg_handle_print_help, &args);
  args.add("-v", "--version", "Print Vektor version and exit", arg_handle_version);
  args.add("-b", "--background", "Run in background (headless) mode", arg_handle_background_mode);
}

int main_args_handle(int argc, const char **argv)
{
  Args args;
  main_args_setup(args);
  return args.parse(argc, argv);
}

}  // namespace vektor::creator
