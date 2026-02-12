#include "VKE_string_ref.hh"
#include <iostream>
#include <vector>

using namespace vektor::vklib;

int main() {
  StringRef s("Hello World");
  Span<char> span = s;

  char buffer[64];
  s.copy_unsafe(buffer);

  std::vector<int> vec = {1, 2, 3};
  MutableSpan<int> mspan(vec);
  mspan[0] = 5;

  for (int i : mspan.index_range()) {
    (void)i;
  }

  std::cout << s << std::endl;
  return 0;
}
