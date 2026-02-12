

#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace vektor {
namespace creator {

struct Args;

using ArgCallback = std::function<int(int argc, const char **argv, void *data)>;

struct ArgDef {
  std::string short_arg;
  std::string long_arg;
  std::string doc;
  ArgCallback callback;
  void *data;
};

class Args {
public:
  void add(const std::string &short_arg, const std::string &long_arg,
           const std::string &doc, ArgCallback callback, void *data = nullptr);
  void print_help();
  int parse(int argc, const char **argv);

private:
  std::vector<ArgDef> args;
};

void main_args_setup(Args &args);
int main_args_handle(int argc, const char **argv);

} // namespace creator
} // namespace vektor
