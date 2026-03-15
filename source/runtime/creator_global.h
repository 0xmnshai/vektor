#pragma once

namespace vektor::creator {
enum GPUBackend {
  GPU_BACKEND_OPENGL,
  GPU_BACKEND_METAL,
};

struct Global {
  bool is_break;
  bool background;

  const char *crashlog_path;
  const char *project_file;

  GPUBackend gpu_backend;
};

extern Global G;
}  // namespace vektor::creator
