#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#ifdef __APPLE__
#  include <mach-o/dyld.h>
#endif

#include "appdir.h"

#define PATH_MAX 1024

namespace vektor::lib {

static char g_executable_dir[PATH_MAX] = "";

void init(const char *argv0)
{
  char path[PATH_MAX];
  uint32_t size = sizeof(path);
  if (_NSGetExecutablePath(path, &size) == 0) {
    char *last_slash = strrchr(path, '/');
    if (last_slash) {
      *last_slash = '\0';
      strncpy(g_executable_dir, path, sizeof(g_executable_dir) - 1);
    }
  }

  if (g_executable_dir[0] == '\0' && argv0) {
    const char *last_slash = strrchr(argv0, '/');
    if (last_slash) {
      size_t len = last_slash - argv0;
      if (len < sizeof(g_executable_dir)) {
        strncpy(g_executable_dir, argv0, len);
        g_executable_dir[len] = '\0';
      }
    }
    else {
      strncpy(g_executable_dir, ".", sizeof(g_executable_dir) - 1);
    }
  }
}

const char *get_user_config_dir()
{
  static char path[PATH_MAX] = "";
  if (path[0] != '\0') {
    return path;
  }

  const char *home = std::getenv("HOME");
  if (!home) {
    return "";
  }

#ifdef __APPLE__
  snprintf(path, sizeof(path), "%s/Library/Application Support/Vektor", home);
#else
  snprintf(path, sizeof(path), "%s/.config/vektor", home);
#endif
  return path;
}

const char *get_user_scripts_dir()
{
  static char path[PATH_MAX] = "";
  if (path[0] != '\0') {
    return path;
  }

  const char *config = get_user_config_dir();
  if (config[0] == '\0') {
    return "";
  }

  snprintf(path, sizeof(path), "%s/scripts", config);
  return path;
}

const char *get_temp_dir()
{
  const char *tmp = std::getenv("TMPDIR");
  if (tmp) {
    return tmp;
  }
  return "/tmp";
}

const char *get_executable_dir()
{
  return g_executable_dir;
}

}  // namespace vektor::lib
