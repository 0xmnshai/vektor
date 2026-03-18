#pragma once

#include "../../dna/DNA_mesh_types.h"

namespace vektor::gpu {

struct GPUMesh {
  enum {
    GPU_BACKEND_OPENGL,
    GPU_BACKEND_METAL,
  } backend = GPU_BACKEND_OPENGL;

  int vertex_count = 0;
  int index_count = 0;

  // OpenGL
  unsigned int vao = 0;
  unsigned int vbo = 0;
  unsigned int ebo = 0;

  // Metal
  void *metal_vbo = nullptr;
  void *metal_ebo = nullptr;
};

GPUMesh *GPU_mesh_create_from_dna_mesh(dna::Mesh *mesh);
void GPU_mesh_free(GPUMesh *gpu_mesh);
void GPU_mesh_draw(GPUMesh *gpu_mesh, void *command_encoder = nullptr);

}  // namespace vektor::gpu
