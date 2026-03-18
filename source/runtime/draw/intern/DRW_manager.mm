#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "../../dna//DNA_uniform.h"
#include "../../gpu/GPU_shader.h"
#include "../../kernel/ecs/ECS_registry.h"
#include "../DRW_manager.hh"
#include "GPU_mesh.h"

#ifdef __APPLE__
#  import <Metal/Metal.h>
#endif

namespace vektor::draw {

void DRW_draw_view(vektor::dna::Scene *scene,
                   void *encoder_or_context,
                   const glm::mat4 &view,
                   const glm::mat4 &projection)
{
  auto &registry = kernel::ECSRegistry::instance().registry();
  auto objects_view = registry.view<dna::Object>();

  for (auto entity : objects_view) {
    auto &obj = registry.get<dna::Object>(entity);

    if (obj.type == dna::ObjectType::Mesh && obj.mesh) {
      // Lazy GPU mesh creation (Could be moved to dependency graph later)
      // Usually stored in object->runtime or a dedicated component
      // Using obj->components for simplicity here. Needs a proper wrapper

      // Let's assume the GPU mesh hasn't been created yet if we're calling this
      // Or we can create a temporary map if we don't want to modify dna::Object directly
      static std::map<void *, gpu::GPUMesh *> mesh_cache;

      if (mesh_cache.find(obj.mesh.get()) == mesh_cache.end()) {
        mesh_cache[obj.mesh.get()] = gpu::GPU_mesh_create_from_dna_mesh(obj.mesh.get());
      }

      gpu::GPUMesh *gpu_mesh = mesh_cache[obj.mesh.get()];

      if (gpu_mesh && obj.shader_program) {
        if (gpu_mesh->backend == gpu::GPUMesh::GPU_BACKEND_METAL && encoder_or_context) {
#ifdef __APPLE__
          auto encoder = (id<MTLRenderCommandEncoder>)encoder_or_context;
          auto *gpu_shader = (gpu::GPUShader *)obj.shader_program;
          auto pipeline = (id<MTLRenderPipelineState>)gpu_shader->metal_pipeline;

          [encoder setRenderPipelineState:pipeline];

          dna::Uniforms uniforms = {};
          auto model = glm::mat4(1.0f);
          model = glm::translate(model, obj.transform.location);
          model = glm::scale(model, obj.transform.scale);
          uniforms.model = model;
          uniforms.view = view;
          uniforms.projection = projection;

          // WARNING: We need view/proj matrices here. Will pass them down from Viewport.
          // Or extract from a Camera entity. To be continued in viewport integration.

          [encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];

          glm::vec4 color = obj.mesh->material.color.to_vec4();
          [encoder setFragmentBytes:&color length:sizeof(color) atIndex:0];

          glm::vec4 lightPos = glm::vec4(10.0f, 15.0f, 10.0f, 1.0f);
          [encoder setFragmentBytes:&lightPos length:sizeof(lightPos) atIndex:2];

          glm::mat4 invView = glm::inverse(view);
          glm::vec4 viewPos = invView[3];
          [encoder setFragmentBytes:&viewPos length:sizeof(viewPos) atIndex:3];

          gpu::GPU_mesh_draw(gpu_mesh, encoder_or_context);
#endif
        }
        else if (gpu_mesh->backend == gpu::GPUMesh::GPU_BACKEND_OPENGL) {
          auto *gpu_shader = (gpu::GPUShader *)obj.shader_program;
          gpu::GPU_shader_bind(gpu_shader);

          auto model = glm::mat4(1.0f);
          model = glm::translate(model, obj.transform.location);
          model = glm::scale(model, obj.transform.scale);

          gpu::GPU_shader_uniform_matrix4(gpu_shader, "model", &model[0][0]);
          gpu::GPU_shader_uniform_matrix4(gpu_shader, "view", &view[0][0]);
          gpu::GPU_shader_uniform_matrix4(gpu_shader, "projection", &projection[0][0]);

          glm::vec4 color = obj.mesh->material.color.to_vec4();
          gpu::GPU_shader_uniform_vector4(gpu_shader, "color", &color[0]);

          glm::vec3 lightPos = glm::vec3(10.0f, 15.0f, 10.0f);
          gpu::GPU_shader_uniform_vector3(gpu_shader, "lightPos", &lightPos[0]);

          glm::mat4 invView = glm::inverse(view);
          glm::vec3 viewPos = glm::vec3(invView[3]);
          gpu::GPU_shader_uniform_vector3(gpu_shader, "viewPos", &viewPos[0]);

          gpu::GPU_mesh_draw(gpu_mesh, encoder_or_context);

          gpu::GPU_shader_unbind(gpu_shader);
        }
      }
    }
  }
}

}  // namespace vektor::draw
