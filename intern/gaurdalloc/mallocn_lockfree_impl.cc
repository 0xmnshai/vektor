
#include "mallocn_intern.hh"
#include <cstdint>

namespace vektor {

static bool malloc_debug_memset = false;

static void (*error_callback)(const char *) = nullptr;

void MEM_lockfree_set_error_callback(void (*func)(const char *)) {
  error_callback = func;
}

bool MEM_lockfree_consistency_check() { return true; }

void MEM_lockfree_set_memory_debug() { malloc_debug_memset = true; }

size_t MEM_lockfree_get_memory_in_use() { return memory_usage_current(); }

uint32_t MEM_lockfree_get_memory_blocks_in_use() {
  return uint32_t(memory_usage_block_num());
}
}