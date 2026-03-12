#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <cstring>

// Forward declarations of test functions
// These will be implemented in their respective test files
extern "C" int vpi_event_test_main(int argc, char **argv);

struct TestDef {
  std::string name;
  int (*func)(int, char **);
};

int main(int argc, char **argv)
{
  bool run_all = false;
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--all") == 0) {
      run_all = true;
    }
  }

  std::vector<TestDef> tests = {
      {"VPI Event Test", reinterpret_cast<int (*)(int, char **)>(vpi_event_test_main)}};

  std::cout << "Starting Vektor Parallel Test Runner..." << std::endl;
  if (!run_all) {
    std::cout << "Note: Running with default settings. Use --all to run all tests." << std::endl;
  }
  std::cout << "Running " << tests.size() << " tests simultaneously." << std::endl;

  std::vector<std::thread> threads;
  threads.reserve(tests.size());
  std::atomic<int> failed_count{0};

  for (const auto &test : tests) {
    threads.emplace_back([&test, &failed_count]() {
      std::cout << "[START] " << test.name << std::endl;
      // We pass --tests to the sub-test to ensure it runs its logic
      const char *sub_argv[] = {test.name.c_str(), "--tests"};
      int result = test.func(2, const_cast<char **>(sub_argv));
      if (result != 0) {
        std::cerr << "[FAIL] " << test.name << " returned " << result << std::endl;
        failed_count++;
      }
      else {
        std::cout << "[PASS] " << test.name << std::endl;
      }
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  if (failed_count > 0) {
    std::cerr << "\nTests failed: " << failed_count << std::endl;
    return 1;
  }

  std::cout << "\nAll tests passed!" << std::endl;
  return 0;
}
