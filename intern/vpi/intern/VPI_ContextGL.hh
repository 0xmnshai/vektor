#pragma once

#include "VPI_Context.hh"
#include "VPI_Types.h"
#include "VPI_Window.hh"

namespace vpi {
class VPI_ContextGL : public VPI_Context {
 public:
  explicit VPI_ContextGL(const VPI_ContextParams &context_params, VPI_Window *window);

  ~VPI_ContextGL() override;

  [[nodiscard]] VPI_TSuccess init_context() const override;

  [[nodiscard]] VPI_TSuccess release_context() const override;

  [[nodiscard]] VPI_TSuccess release_native_handles() const override;

 protected:
  static int s_shared_count_;
};
}  // namespace vpi
