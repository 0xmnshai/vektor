#include <chrono>
#include <iostream>
#include <thread>

#include "VPI_ISystem.h"
#include "VPI_QtEventConsumer.hh"
#include "intern/VPI_Event.hh"

class TestEventConsumer : public vpi::VPI_QtEventConsumer {
 public:
  VPI_TSuccess consume_event(vpi::VPI_Event *event) const noexcept override
  {
    std::cout << "Received event of type: " << event->get_type() << std::endl;

    if (event->get_type() == VPI_kKey) {
      auto *ke = dynamic_cast<vpi::VPI_KeyEvent *>(event);
      if (ke) {
        std::cout << "  Key: " << ke->data.key << std::endl;
      }
    }
    else if (event->get_type() == VPI_kMouseButton) {
      auto *me = dynamic_cast<vpi::VPI_MouseEvent *>(event);
      if (me) {
        std::cout << "  Button: " << me->data.button << " at (" << me->data.x << ", " << me->data.y
                  << ")" << std::endl;
      }
    }

    (void)event->consume();
    return VPI_kSuccess;
  }
};

extern "C" int vpi_event_test_main(int argc, char **argv)
{
  bool should_run = false;
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "--tests") {
      should_run = true;
      break;
    }
  }

  if (!should_run) {
    std::cout << "VPI Event Test: Use --tests to run." << std::endl;
    return 0;
  }

  (void)vpi::VPI_ISystem::create();
  vpi::VPI_ISystem *system = vpi::VPI_ISystem::get();
  (void)system->init();

  vpi::VPI_Window *window = system->create_window("VPI Event Test", 100, 100, 800, 600, nullptr);

  TestEventConsumer consumer;
  (void)window->add_event_consumer(&consumer);

  std::cout << "Event System Test Started. Interact with the window to see events." << std::endl;
  std::cout << "Triggering programmatic resize..." << std::endl;
  (void)window->set_size(1024, 768);
  std::cout << "The test will run for 10 seconds..." << std::endl;

  auto start = std::chrono::steady_clock::now();
  while (std::chrono::steady_clock::now() - start < std::chrono::seconds(10)) {
    // Process events for a short duration
    (void)window->process_events(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  bool is_running = false;
  (void)system->exit(is_running);
  (void)vpi::VPI_ISystem::dispose();

  return 0;
}
