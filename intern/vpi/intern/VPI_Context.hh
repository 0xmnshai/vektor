#pragma once

#include "../VPI_IContext.h"
#include "VPI_Window.hh"
#include "../VPI_Types.h"

namespace vpi {
class VPI_Context : public VPI_IContext {
 public:
  explicit VPI_Context(const VPI_ContextParams &context_params, VPI_Window *window)
      : context_params_(context_params), window_(window)
  {
  }

  ~VPI_Context() override
  {
    if (s_active_context_ == this) {
      s_active_context_ = nullptr;
    }
  };

  static inline VPI_Context *get_active_context()
  {
    return s_active_context_;
  }

  [[nodiscard]] VPI_TSuccess init_context() const override
  {
    return VPI_TSuccess::VPI_kSuccess;
  }

  [[nodiscard]] VPI_TSuccess release_context() const override
  {
    return VPI_TSuccess::VPI_kSuccess;
  }

  [[nodiscard]] VPI_TSuccess release_native_handles() const override
  {
    return VPI_TSuccess::VPI_kSuccess;
  }

  void resize_context(uint32_t /*width*/, uint32_t /*height*/) const override {}

  void *get_user_data()
  {
    return user_data_;
  }

  void get_user_data(void *user_data)
  {
    user_data_ = user_data;
  }

  [[nodiscard]] VPI_Window *get_window() const override
  {
    return window_;
  }

 protected:
  static thread_local inline VPI_Context *s_active_context_;
  VPI_Window *window_;
  VPI_ContextParams context_params_;
  void *user_data_ = nullptr;
};
}  // namespace vpi
