
#pragma once

#include <cstdio>

namespace vektor {
namespace creator {

struct VKA_ArgCallback_Deferred;
struct vkArgs;
struct vkContext;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WITH_PYTHON_MODULE

void main_srgs_setup(struct vkContext *vkC, struct vkArgs *vkArgs, bool all);

int main_args_handle_load_file(int argc, const char **argv, void *data);

void main_signal_setup(void);
void main_signal_setup_background(void);
void main_signal_setup_fpe(void);

#endif

struct ApplicationState {
  struct {
    bool use_crash_handler;
    bool use_abort_handler;
  } signal;

  struct {
    unsigned char python;
  } exit_code_on_error;

  struct VKA_ArgCallback_Deferred *main_arg_deferred;
};

extern struct ApplicationState app_state;

enum {
  ARG_PASS_ENVIRONMENT = 1,
  ARG_PASS_SETTINGS = 2,
  ARG_PASS_SETTINGS_GUI = 3,
  ARG_PASS_SETTINGS_FORCE = 4,
  ARG_PASS_FINAL = 5,
};

static void callback_mem_error(const char *errorStr) {
  fputs(errorStr, stderr);
  fflush(stderr);
}

#define VEKTOR_VERSION_FMT "Vektor %d.%d.%d"
#define VEKTOR_VERSION_ARG                                                     \
  (VEKTOR_VERSION / 100), (VEKTOR_VERSION % 100), VEKTOR_VERSION_PATCH

#define BUILD_DATE

#ifdef BUILD_DATE
extern char build_date[];
extern char build_time[];
extern char build_hash[];
extern unsigned long build_commit_timestamp;

extern char build_commit_date[16];
extern char build_commit_time[16];

extern char build_branch[];
extern char build_platform[];
extern char build_type[];
extern char build_cflags[];
extern char build_cxxflags[];
extern char build_linkflags[];
extern char build_system[];
#endif

#ifdef __cplusplus
}
#endif
}
}