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
  bool needs_main_thread;
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
      {"VPI Event Test", reinterpret_cast<int (*)(int, char **)>(vpi_event_test_main), true}};

  std::cout << "Starting Vektor Parallel Test Runner..." << std::endl;
  if (!run_all) {
    std::cout << "Note: Running with default settings. Use --all to run all tests." << std::endl;
  }
  std::cout << "Running " << tests.size() << " tests." << std::endl;

  std::vector<std::thread> parallel_threads;
  std::atomic<int> failed_count{0};

  // 1. Start tests that can run in background threads
  for (const auto &test : tests) {
    if (!test.needs_main_thread) {
      parallel_threads.emplace_back([&test, &failed_count]() {
        std::cout << "[START] " << test.name << " (Parallel Thread)" << std::endl;
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
  }

  // 2. Run tests that MUST run on the main thread
  for (const auto &test : tests) {
    if (test.needs_main_thread) {
      std::cout << "[START] " << test.name << " (Main Thread)" << std::endl;
      const char *sub_argv[] = {test.name.c_str(), "--tests"};
      int result = test.func(2, const_cast<char **>(sub_argv));
      if (result != 0) {
        std::cerr << "[FAIL] " << test.name << " returned " << result << std::endl;
        failed_count++;
      }
      else {
        std::cout << "[PASS] " << test.name << std::endl;
      }
    }
  }

  // 3. Wait for all parallel tests to complete
  for (auto &t : parallel_threads) {
    t.join();
  }

  if (failed_count > 0) {
    std::cerr << "\nTests failed: " << failed_count << std::endl;
    return 1;
  }

  std::cout << "\nAll tests passed!" << std::endl;
  return 0;
}
