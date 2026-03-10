#pragma  once 

#include "VPI_IContext.h"
#include "VPI_Types.h"

namespace vpi {
class VPI_Context : public VPI_IContext {
 public:
  explicit VPI_Context(const VPI_ContextParams &context_params) : context_params_(context_params)
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

  [[nodiscard]] VPI_TSuccess activate_context() const override = 0;

  [[nodiscard]] VPI_TSuccess release_context() const override = 0;

  [[nodiscard]] virtual VPI_TSuccess initialise_context() const = 0;

  virtual VPI_TSuccess release_native_handles() = 0;

  void *get_user_data()
  {
    return user_data_;
  }

  void get_user_data(void *user_data)
  {
    user_data_ = user_data;
  }

 protected:
  static thread_local inline VPI_Context *s_active_context_;
  VPI_ContextParams context_params_;
  void *user_data_ = nullptr;
};
}  // namespace vpi
