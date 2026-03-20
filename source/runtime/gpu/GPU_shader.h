#pragma once

#include <QOpenGLShaderProgram>

#include "../../intern/clog/CLG_log.h"

namespace vektor::gpu {

CLG_LOGREF_DECLARE_EXTERN(LOG_SHADER);

#define GL_SHADER_STORAGE_BUFFER 0x90D2

typedef struct GPUShader {
  enum {
    GPU_BACKEND_OPENGL,
    GPU_BACKEND_METAL,
  } backend;

  // for opengl
  QOpenGLShaderProgram *program;
  // for metal (MTL::RenderPipelineState*, etc. using void* to avoid header mess)
  void *metal_pipeline;
} GPUShader;

typedef struct GPUShaderSourceParameters {
  const char *vert_entry; /* Optional: for Metal (defaults to main) */
  const char *frag_entry; /* Optional: for Metal (defaults to main) */
} GPUShaderSourceParameters;

GPUShader *GPU_shader_create_from_slang(const char *vert_path, const char *frag_path);
GPUShader *GPU_shader_create_from_source(const char *vert_path,
                                         const char *frag_path,
                                         const GPUShaderSourceParameters *params = nullptr);

GPUShader *GPU_shader_create_from_strings(const char *vert_src,
                                          const char *frag_src,
                                          const GPUShaderSourceParameters *params = nullptr);

QOpenGLShaderProgram *GPU_shader_get_program(GPUShader *shader);
static void print_compute_results();

void GPU_shader_bind(GPUShader *shader);
void GPU_shader_bind_metal(GPUShader *shader, void *encoder);
void GPU_shader_unbind(GPUShader *shader);
void GPU_shader_free(GPUShader *shader);

void GPU_shader_uniform_float(GPUShader *shader, const char *name, float val);
void GPU_shader_uniform_int(GPUShader *shader, const char *name, int val);
void GPU_shader_uniform_vector3(GPUShader *shader, const char *name, const float val[3]);
void GPU_shader_uniform_vector4(GPUShader *shader, const char *name, const float val[4]);
void GPU_shader_uniform_matrix4(GPUShader *shader, const char *name, const float val[16]);
void GPU_shader_uniform_matrix4_array(GPUShader *shader, const char *name, const float *val, int count);
void GPU_shader_uniform_texture(GPUShader *shader, const char *name, int slot);

}  // namespace vektor::gpu
