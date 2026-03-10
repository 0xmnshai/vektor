#pragma once

#include "VPI_Types.h"
#include "intern/VPI_Context.hh"
#include "intern/VPI_Window.hh"

namespace vpi {
class VPI_ContextGL : public VPI_Context {
  explicit VPI_ContextGL(const VPI_ContextParams &context_params, VPI_Window *window);

  ~VPI_ContextGL() override;

  [[nodiscard]] VPI_TSuccess activate_context() const override = 0;

  [[nodiscard]] VPI_TSuccess release_context() const override = 0;

  [[nodiscard]] virtual VPI_TSuccess initialise_context() const override = 0;

  virtual VPI_TSuccess release_native_handles() override = 0;

 protected:
  VPI_Window *window_;
  static int s_shared_count_;
};
}  // namespace vpi
