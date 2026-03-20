#include <QOpenGLFunctions_4_1_Core>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "../../dna//DNA_uniform.h"
#include "../../gpu/GPU_shader.h"
#include "../../gpu/shaders/SHDR_outline.h"
#include "../../kernel/ecs/ECS_registry.h"
#include "../../lib/intern/appdir.h"
#include "../../runtime/creator_global.h"
#include "../DRW_manager.hh"
#include "GPU_mesh.h"
#include "SHDR_outline.h"

#ifdef __APPLE__
#  import <Metal/Metal.h>
#endif

namespace vektor::draw {

void DRW_draw_view(vektor::dna::Scene *scene,
                   void *encoder_or_context,
                   const glm::mat4 &view,
                   const glm::mat4 &projection,
                   int width,
                   int height,
                   float time)
{
  auto &registry = kernel::ECSRegistry::instance().registry();
  auto objects_view = registry.view<dna::Object>();

  for (auto entity : objects_view) {
    auto &obj = registry.get<dna::Object>(entity);

    if (obj.type == dna::ObjectType::Mesh && obj.mesh) {
      bool is_selected = (obj.select_flag & dna::BASE_SELECTED);
      if (!is_selected && registry.any_of<dna::Selected>(entity)) {
        is_selected = registry.get<dna::Selected>(entity).selected;
      }

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

          glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
          if (!obj.mesh->materials.empty() && obj.mesh->materials[0]) {
            color = obj.mesh->materials[0]->color.to_vec4();
          }
          [encoder setFragmentBytes:&color length:sizeof(color) atIndex:0];

          gpu::GPU_mesh_draw(gpu_mesh, encoder_or_context);

          if (is_selected) {
            // Metal: Draw the outline shell
            static gpu::OutlineShader *outline_shader = nullptr;
            if (!outline_shader) {
              outline_shader = new gpu::OutlineShader();
              QString shader_path = QString(vektor::lib::get_application_dir_path()) +
                                    "/../../source/runtime/gpu/shaders/core";
              outline_shader->init(shader_path);
            }
            if (outline_shader) {
              outline_shader->draw(projection, view, model, width, height, time);
              gpu::GPU_mesh_draw(gpu_mesh, encoder_or_context);
            }
          }
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

          glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
          if (!obj.mesh->materials.empty() && obj.mesh->materials[0]) {
            color = obj.mesh->materials[0]->color.to_vec4();
          }
          float color_arr[3] = {color.x, color.y, color.z};
          gpu::GPU_shader_uniform_vector3(gpu_shader, "color", color_arr);

          // created for outline ?
          QOpenGLFunctions_4_1_Core gl;
          gl.initializeOpenGLFunctions();

          if (is_selected) {
            gl.glEnable(GL_STENCIL_TEST);
            gl.glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil for THIS object
            gl.glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            gl.glStencilFunc(GL_ALWAYS, 1, 0xFF);
            gl.glStencilMask(0xFF);
          }

          gpu::GPU_mesh_draw(gpu_mesh, encoder_or_context);

          if (is_selected) {
            // Immediately draw the outline for THIS object
            gl.glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            gl.glStencilMask(0x00);
            gl.glDisable(GL_DEPTH_TEST);

            static gpu::OutlineShader *outline_shader = nullptr;
            if (!outline_shader) {
              outline_shader = new gpu::OutlineShader();
              QString shader_path = QString(vektor::lib::get_application_dir_path()) +
                                    "/../../source/runtime/gpu/shaders/core";
              outline_shader->init(shader_path);
            }
            if (outline_shader) {
              outline_shader->draw(projection, view, model, width, height, time);
              gpu::GPU_mesh_draw(gpu_mesh, encoder_or_context);
            }

            gl.glEnable(GL_DEPTH_TEST);
            gl.glDisable(GL_STENCIL_TEST);
          }

          gpu::GPU_shader_unbind(gpu_shader);
        }
      }
    }
  }
}
}  // namespace vektor::draw
