#include <iostream>
#include <utility>

#include "../../intern/clog/CLG_log.h"
#include "creator_args.hh"
#include "creator_global.h"
#include "kernel/intern/vektor.hh"

namespace vektor::creator {

CLG_LOGREF_DECLARE_GLOBAL(V_LOG, "runtime.args");

void Args::add(
    const char *short_arg, const char *long_arg, const char *doc, ArgCallback callback, void *data)
{
  args.push_back(ArgDef{short_arg, long_arg, doc, std::move(callback), data});
}

void Args::print_help()
{
  CLOG_INFO(V_LOG, "Vektor Engine Runtime\n");
  CLOG_INFO(V_LOG, "Usage: vektor runtime [args ...]\n\n");

  for (const auto &arg : args) {
    CLOG_INFO(V_LOG, "  ");
    if (arg.short_arg) {
      std::cout << arg.short_arg;
      if (arg.long_arg) {
        std::cout << ", ";
      }
    }
    if (arg.long_arg) {
      std::cout << arg.long_arg;
    }

    size_t len = (arg.short_arg ? std::strlen(arg.short_arg) : 0) +
                 (arg.short_arg && arg.long_arg ? 2 : 0) +
                 (arg.long_arg ? std::strlen(arg.long_arg) : 0);

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
        CLOG_ERROR(V_LOG, "Unknown argument: %s", argv[i]);
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
  std::cout << "Build Date: " << __DATE__ << "\n";
  std::cout << "Build Time: " << __TIME__ << "\n";

  std::cout << "Version: " << kernel::VKE_vektor_version_string() << "\n";

  exit(0);
  return 0;
}

static int arg_handle_background_mode(int, const char **, void *)
{
  CLOG_INFO(V_LOG, "Running in background mode (headless).");
  G.background = true;
  return 0;
}

static int arg_handle_tests(int, const char **, void *)
{
  std::cout << "To run tests, please execute the 'tests_main' binary in the build directory.\n";
  std::cout << "Usage: ./bin/tests_main [--all]\n";
  exit(0);
  return 0;
}

void main_args_setup(Args &args)
{
  args.add("-h", "--help", "Print this help text and exit", arg_handle_print_help, &args);
  args.add("-v", "--version", "Print Vektor version and exit", arg_handle_version);
  args.add("-b", "--background", "Run in background (headless) mode", arg_handle_background_mode);
  args.add("", "--tests", "Show information on how to run tests", arg_handle_tests);
}

int main_args_handle(int argc, const char **argv)
{
  Args args;
  main_args_setup(args);
  return args.parse(argc, argv);
}

}  // namespace vektor::creator
