#include <filesystem>

#include "CLG_init.hh"
#include "CLG_log.h"

namespace clog {
void clog_init(const char *id, const char *file_name, const char *var)
{
  clog::CLG_init();
  clog::CLG_level_set(clog::CLG_LEVEL_INFO);
  clog::CLG_output_use_timestamp_set(1);

  std::filesystem::path log_dir = std::filesystem::path(VEKTOR_SOURCE_DIR) / "logs";
  std::filesystem::create_directories(log_dir);
  std::filesystem::path log_path = log_dir / file_name;

  FILE *log_file = fopen(log_path.c_str(), "a");
  if (log_file) {
    clog::CLG_output_extra_set(log_file);
  }

  CLG_LOGREF_DECLARE_GLOBAL(V_LOG, id);
  CLOG_INFO(V_LOG, "%s", var);
}

void clg_exit()
{
  clog::CLG_exit();
}
}  // namespace clog