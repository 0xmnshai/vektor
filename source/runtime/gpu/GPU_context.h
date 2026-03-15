#pragma once

#include "../../intern/vpi/intern/VPI_Context.hh"

namespace vektor::gpu {

class GPUContext {
 public:
  GPUContext();
  ~GPUContext();
};

class GPUSecondaryContext {
 private:
  vpi::VPI_Context *ghost_context_;
  GPUContext *gpu_context_;

 public:
  GPUSecondaryContext() = default;
  ~GPUSecondaryContext() = default;

  /** Must be called from a secondary thread. */
  void activate();
};

}  // namespace vektor::gpu
