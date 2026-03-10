#pragma once

namespace vektor::creator {
struct Global {
  bool is_break;
  bool background;

  const char *crashlog_path;
  const char *project_file;
};

extern Global G;
}  // namespace vektor::creator
