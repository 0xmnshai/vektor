#include "creator_args.h"
#include <iostream>

int main() {
  using namespace vektor::creator;

  std::cout << "--- Test 1: Help ---\n";
  const char *argv_help[] = {"vektor", "--help"};

  Args args;
  main_args_setup(args);

  std::cout << "--- Test 2: Window Geometry ---\n";
  const char *argv_geom[] = {"vektor", "--window-geometry", "10", "20", "800",
                             "600"};
  int result = args.parse(6, argv_geom);
  std::cout << "Parse result: " << result << "\n";

  std::cout << "--- Test 3: Unknown Arg ---\n";
  const char *argv_unknown[] = {"vektor", "--unknown"};
  args.parse(2, argv_unknown);

  std::cout << "--- Test 4: Combined ---\n";
  const char *argv_combined[] = {"vektor", "-b", "--log-level", "debug", "-d"};
  args.parse(5, argv_combined);

  return 0;
}
