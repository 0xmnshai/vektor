#include <algorithm>
#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <vector>

#include "mallocn_intern.hh"

namespace vektor {

struct Local;
struct Global;

struct alignas(128) Local {
  std::shared_ptr<Global> global;

  bool destructed = false;
  bool is_main = false;
  std::atomic<int64_t> mem_in_use{0};
  std::atomic<int64_t> blocks_num{0};
  std::atomic<int64_t> mem_in_use_during_peak_update{0};

  Local();
  ~Local();
};

struct Global {
  std::mutex locals_mutex;
  std::vector<Local *> locals;
  std::atomic<int64_t> mem_in_use_outside_locals{0};
  std::atomic<int64_t> blocks_num_outside_locals{0};
  std::atomic<size_t> peak{0};
};

}

static std::atomic<bool> use_local_counters{true};
static constexpr int64_t peak_update_threshold = 1024 * 1024;

static std::shared_ptr<vektor::Global> &get_global_ptr() {
  static std::shared_ptr<vektor::Global> global =
      std::make_shared<vektor::Global>();
  return global;
}

static vektor::Global &get_global() { return *get_global_ptr(); }

static vektor::Local &get_local_data() {
  static thread_local vektor::Local local;
  assert(!local.destructed);
  return local;
}

vektor::Local::Local() {
  this->global = get_global_ptr();

  std::lock_guard<std::mutex> lock{this->global->locals_mutex};
  if (this->global->locals.empty()) {
    this->is_main = true;
  }
  this->global->locals.push_back(this);
}

vektor::Local::~Local() {
  std::lock_guard<std::mutex> lock{this->global->locals_mutex};

  this->global->locals.erase(std::find(this->global->locals.begin(),
                                       this->global->locals.end(), this));
  this->global->blocks_num_outside_locals.fetch_add(this->blocks_num,
                                                    std::memory_order_relaxed);
  this->global->mem_in_use_outside_locals.fetch_add(this->mem_in_use,
                                                    std::memory_order_relaxed);

  if (this->is_main) {
    use_local_counters.store(false, std::memory_order_relaxed);
  }
  this->destructed = true;
}

size_t memory_usage_current() {
  vektor::Global &global = get_global();
  std::lock_guard<std::mutex> lock{global.locals_mutex};


  int64_t mem_in_use = global.mem_in_use_outside_locals;
  for (const vektor::Local *local : global.locals) {
    mem_in_use += local->mem_in_use;
  }
  return size_t(mem_in_use);
}

static void update_global_peak() {
  vektor::Global &global = get_global();
  global.peak = std::max<size_t>(global.peak, memory_usage_current());

  std::lock_guard<std::mutex> lock{global.locals_mutex};

  for (vektor::Local *local : global.locals) {
    assert(!local->destructed);
    local->mem_in_use_during_peak_update =
        local->mem_in_use.load(std::memory_order_relaxed);
  }
}

size_t memory_usage_peak() {
  update_global_peak();
  vektor::Global &global = get_global();
  return global.peak;
}

void memory_usage_peak_reset() {
  vektor::Global &global = get_global();
  global.peak = memory_usage_current();
}

void memory_usage_init() { get_local_data(); }

void memory_usage_block_alloc(const size_t size) {
  if (LIKELY(use_local_counters.load(std::memory_order_relaxed))) {
    vektor::Local &local = get_local_data();
    local.blocks_num.fetch_add(1, std::memory_order_relaxed);
    local.mem_in_use.fetch_add(int64_t(size), std::memory_order_relaxed);

    if (local.mem_in_use - local.mem_in_use_during_peak_update >
        peak_update_threshold) {
      update_global_peak();
    }
  } else {
    vektor::Global &global = get_global();
    global.blocks_num_outside_locals.fetch_add(1, std::memory_order_relaxed);
    global.mem_in_use_outside_locals.fetch_add(int64_t(size),
                                               std::memory_order_relaxed);
  }
}

void memory_usage_block_free(const size_t size) {
  if (LIKELY(use_local_counters)) {
    vektor::Local &local = get_local_data();
    local.mem_in_use.fetch_sub(int64_t(size), std::memory_order_relaxed);
    local.blocks_num.fetch_sub(1, std::memory_order_relaxed);
  } else {
    vektor::Global &global = get_global();
    global.blocks_num_outside_locals.fetch_sub(1, std::memory_order_relaxed);
    global.mem_in_use_outside_locals.fetch_sub(int64_t(size),
                                               std::memory_order_relaxed);
  }
}

size_t memory_usage_block_num() {
  vektor::Global &global = get_global();
  std::lock_guard<std::mutex> lock{global.locals_mutex};

  int64_t blocks_num = global.blocks_num_outside_locals;
  for (const vektor::Local *local : global.locals) {
    blocks_num += local->blocks_num;
  }
  return size_t(blocks_num);
}