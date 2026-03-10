#pragma once

namespace vektor::creator {

#ifdef __cplusplus
extern "C" {

struct ApplicationState {
  struct {
    bool use_crash_handler;
    bool use_abort_handler;
  } signal;

  struct {
    unsigned char python;
  } exit_code_on_error;
};

extern struct ApplicationState app_state;
}
#endif
}  // namespace vektor::creator
