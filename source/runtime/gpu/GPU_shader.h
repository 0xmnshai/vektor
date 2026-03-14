#pragma once

#include <QOpenGLShaderProgram>

namespace vektor::gpu {

#define GL_SHADER_STORAGE_BUFFER 0x90D2

typedef struct GPUShader {
  QOpenGLShaderProgram *program;

} GPUShader;

GPUShader *GPU_shader_create_from_slang(const char *vert_path, const char *frag_path);

QOpenGLShaderProgram *GPU_shader_get_program(GPUShader *shader);

static void print_compute_results();

void GPU_shader_bind(GPUShader *shader);

void GPU_shader_unbind(GPUShader *shader);

void GPU_shader_free(GPUShader *shader);

void GPU_shader_uniform_float(GPUShader *shader, const char *name, float val);
void GPU_shader_uniform_vector3(GPUShader *shader, const char *name, const float val[3]);
void GPU_shader_uniform_matrix4(GPUShader *shader, const char *name, const float val[16]);

}  // namespace vektor::gpu
