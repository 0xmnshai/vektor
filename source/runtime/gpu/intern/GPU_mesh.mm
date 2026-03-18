
#include <OpenGL/gltypes.h>

#ifdef __APPLE__
#  include "../../../intern/vpi/intern/VPI_ContextMTL.hh"
#  import <Metal/Metal.h>
#endif

#include <QOpenGLFunctions_4_1_Core>
#include <vector>

#include "../../creator_global.h"
#include "../../dna/DNA_vertex_.h"
#include "../GPU_mesh.h"

namespace vektor::gpu {

using namespace dna;

GPUMesh *GPU_mesh_create_from_dna_mesh(dna::Mesh *mesh)
{
  if (!mesh || mesh->faces_num == 0)
    return nullptr;

  auto *gpu_mesh = new GPUMesh();
  gpu_mesh->backend = (creator::G.gpu_backend == creator::GPU_BACKEND_METAL) ?
                          GPUMesh::GPU_BACKEND_METAL :
                          GPUMesh::GPU_BACKEND_OPENGL;
  gpu_mesh->vertex_count = mesh->verts_num;

  // We need to unpack the mesh into a format suitable for the GPU (triangles)
  // For simplicity, assuming the primitive generation outputs triangulated faces,
  // or simple quads that we can easily split.

  std::vector<dna::GPUVertex> vertices;
  std::vector<uint32_t> indices;
  vertices.reserve(mesh->verts_num);

  for (int i = 0; i < mesh->verts_num; i++) {
    vertices.push_back({mesh->mvert[i].co, mesh->mvert[i].no});
  }

  for (int i = 0; i < mesh->faces_num; i++) {
    int first = mesh->mpoly[i].first_corner;
    int count = mesh->mpoly[i].num_corners;

    if (count == 3) {
      indices.push_back(mesh->mloop[first].v);
      indices.push_back(mesh->mloop[first + 1].v);
      indices.push_back(mesh->mloop[first + 2].v);
    }
    else if (count == 4) {
      // Triangulate quad (0-1-2-3) -> (0-1-2) + (0-2-3)
      indices.push_back(mesh->mloop[first].v);
      indices.push_back(mesh->mloop[first + 1].v);
      indices.push_back(mesh->mloop[first + 2].v);

      indices.push_back(mesh->mloop[first].v);
      indices.push_back(mesh->mloop[first + 2].v);
      indices.push_back(mesh->mloop[first + 3].v);
    }
  }

  gpu_mesh->index_count = (int)indices.size();

  if (gpu_mesh->backend == GPUMesh::GPU_BACKEND_METAL) {
#ifdef __APPLE__
    id<MTLDevice> device = (id<MTLDevice>)vpi::VPI_ContextMTL::get_current_device();
    if (device) {
      id<MTLBuffer> vbo = [device newBufferWithBytes:vertices.data()
                                              length:vertices.size() * sizeof(GPUVertex)
                                             options:MTLResourceStorageModeShared];
      id<MTLBuffer> ebo = [device newBufferWithBytes:indices.data()
                                              length:indices.size() * sizeof(uint32_t)
                                             options:MTLResourceStorageModeShared];
      gpu_mesh->metal_vbo = (void *)vbo;
      gpu_mesh->metal_ebo = (void *)ebo;
    }
#endif
  }
  else {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();

    gl.glGenVertexArrays(1, &gpu_mesh->vao);
    gl.glGenBuffers(1, &gpu_mesh->vbo);
    gl.glGenBuffers(1, &gpu_mesh->ebo);

    gl.glBindVertexArray(gpu_mesh->vao);

    auto vbo_size = vertices.size() * sizeof(GPUVertex);
    auto ebo_size = indices.size() * sizeof(uint32_t);

    assert(vbo_size <= std::numeric_limits<GLsizeiptr>::max());
    assert(ebo_size <= std::numeric_limits<GLsizeiptr>::max());

    gl.glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo);
    gl.glBufferData(
        GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vbo_size), vertices.data(), GL_STATIC_DRAW);

    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->ebo);
    gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    static_cast<GLsizeiptr>(ebo_size),
                    indices.data(),
                    GL_STATIC_DRAW);

    gl.glEnableVertexAttribArray(0);
    gl.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void *)0);

    gl.glEnableVertexAttribArray(1);
    gl.glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void *)(sizeof(glm::vec3)));

    gl.glBindVertexArray(0);
  }

  return gpu_mesh;
}

void GPU_mesh_free(GPUMesh *gpu_mesh)
{
  if (!gpu_mesh)
    return;

  if (gpu_mesh->backend == GPUMesh::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glDeleteBuffers(1, &gpu_mesh->vbo);
    gl.glDeleteBuffers(1, &gpu_mesh->ebo);
    gl.glDeleteVertexArrays(1, &gpu_mesh->vao);
  }
#ifdef __APPLE__
  else if (gpu_mesh->backend == GPUMesh::GPU_BACKEND_METAL) {
    // ARC usually handles this, but since we are casting to void*,
    // we might need strict bridging if not completely handled by the caller.
    // For now, assume ARC handles it if bridged correctly or we manually release.
    // id<MTLBuffer> vbo = (__bridge_transfer id<MTLBuffer>)gpu_mesh->metal_vbo;
    // vbo = nil;
  }
#endif

  delete gpu_mesh;
}

void GPU_mesh_draw(GPUMesh *gpu_mesh, void *command_encoder)
{
  if (!gpu_mesh)
    return;

  if (gpu_mesh->backend == GPUMesh::GPU_BACKEND_METAL && command_encoder) {
#ifdef __APPLE__
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)command_encoder;
    [encoder setVertexBuffer:(id<MTLBuffer>)gpu_mesh->metal_vbo offset:0 atIndex:0];
    [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                        indexCount:gpu_mesh->index_count
                         indexType:MTLIndexTypeUInt32
                       indexBuffer:(id<MTLBuffer>)gpu_mesh->metal_ebo
                 indexBufferOffset:0];
#endif
  }
  else if (gpu_mesh->backend == GPUMesh::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glBindVertexArray(gpu_mesh->vao);
    gl.glDrawElements(GL_TRIANGLES, gpu_mesh->index_count, GL_UNSIGNED_INT, 0);
    gl.glBindVertexArray(0);
  }
}

}  // namespace vektor::gpu
