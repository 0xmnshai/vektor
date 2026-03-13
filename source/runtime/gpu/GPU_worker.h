#pragma once

#include <memory>
#include <thread>
#include <vector>

#include "gpu/GPU_context.h"

namespace vektor::gpu {

using WorkCallback = void (*)(void *);
using WorkID = uint64_t;

class GPUWorker {
 private:
  std::vector<std::unique_ptr<std::thread>> threads_;
  std::queue<void *> thread_queue_;
  WorkCallback callback_;

 public:
  enum class ContextType {
    Main,
    PerThread,
  };

  enum class ThreadQueueWorkPriority {
    Low,
    Normal,
    High,
  };

  GPUWorker(uint32_t threads_count, ContextType context_type, WorkCallback callback);
  ~GPUWorker();

  WorkID push_work(void *work, ThreadQueueWorkPriority priority);
  bool cancel_work(WorkID id);
  bool is_empty();

  void run(std::shared_ptr<GPUSecondaryContext> context);
};
}  // namespace vektor::gpu
