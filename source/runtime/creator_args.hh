#pragma once

#include <functional>
#include <vector>

namespace vektor::creator {

using ArgCallback = std::function<int(int argc, const char **argv, void *data)>;

struct ArgDef {
  const char *short_arg;
  const char *long_arg;
  const char *doc;
  ArgCallback callback;
  void *data;
};

class Args {
 public:
  void add(const char *short_arg,
           const char *long_arg,
           const char *doc,
           ArgCallback callback,
           void *data = nullptr);

  void print_help();

  int parse(int argc, const char **argv);

 private:
  std::vector<ArgDef> args;
};

void main_args_setup(Args &args);

int main_args_handle(int argc, const char **argv);

static int arg_handle_print_help(int, const char **, void *data);

static int arg_handle_version(int, const char **, void *);

static int arg_handle_background_mode(int, const char **, void *);

}  // namespace vektor::creator
