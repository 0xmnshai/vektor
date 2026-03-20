#include <QOpenGLFunctions_4_1_Core>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "../../dna//DNA_uniform.h"
#include "../../gpu/GPU_framebuffer.h"
#include "../../gpu/GPU_shader.h"
#include "../../gpu/shaders/SHDR_outline.h"
#include "../../kernel/ecs/ECS_registry.h"
#include "../../lib/intern/appdir.h"
#include "../../runtime/creator_global.h"
#include "../DRW_manager.hh"
#include "DNA_light.h"
#include "GPU_mesh.h"
#include "SHDR_outline.h"

#ifdef __APPLE__
#  import <Metal/Metal.h>
#endif

namespace vektor::draw {

struct GPULight {
  int type;
  float _pad0[3];
  glm::vec3 position;
  float _pad1;
  glm::vec3 color;
  float energy;
  float range;
  float _pad2[3]; // Pad to 64 bytes (16-byte aligned)
};

#define MAX_LIGHTS 8

struct LightingUniforms {
  int num_lights;
  float _pad0[3];
  GPULight lights[MAX_LIGHTS];
};

static gpu::GPUShader *get_shadow_shader()
{
  static gpu::GPUShader *shdr = nullptr;
  if (!shdr) {
    QString shader_path = QString(vektor::lib::get_application_dir_path()) +
                          "/../../source/runtime/gpu/shaders/core/shadow/shadow";
    shdr = gpu::GPU_shader_create_from_source((shader_path + ".vert").toUtf8().constData(),
                                              (shader_path + ".frag").toUtf8().constData());
  }
  return shdr;
}

static gpu::GPUFrameBuffer *get_shadow_fb()
{
  static gpu::GPUFrameBuffer *fb = nullptr;
  if (!fb) {
    fb = gpu::GPU_framebuffer_create_depth_only(2048, 2048);
  }
  return fb;
}

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

  // 1. Gather active lights in the scene
  LightingUniforms lighting = {};
  lighting.num_lights = 0;
  for (auto entity : objects_view) {
    auto &l_obj = registry.get<dna::Object>(entity);
    if (l_obj.type == dna::ObjectType::Light && l_obj.light && lighting.num_lights < MAX_LIGHTS) {
      auto &la = *l_obj.light;
      GPULight &gl = lighting.lights[lighting.num_lights++];
      gl.type = (int)la.type;
      gl.position = l_obj.transform.location;
      gl.color = la.color;
      gl.energy = la.energy;
      gl.range = la.distance;
    }
  }

  // 2. Shadow Pass (OpenGL Only for now)
  const bool is_opengl = (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL);
  glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

  if (is_opengl && lighting.num_lights > 0) {
    auto *shadow_fb = get_shadow_fb();
    auto *shadow_shdr = get_shadow_shader();

    if (shadow_fb && shadow_shdr) {
      gpu::GPU_framebuffer_bind(shadow_fb);
      QOpenGLFunctions_4_1_Core gl;
      gl.initializeOpenGLFunctions();
      gl.glViewport(0, 0, shadow_fb->width, shadow_fb->height);
      gl.glClear(GL_DEPTH_BUFFER_BIT);

      gpu::GPU_shader_bind(shadow_shdr);

      glm::vec3 lightPos = lighting.lights[0].position;
      // Using an orthographic projection centered at (0,0,0) for the shadow map
      // This is often better for a "top-down" directional-like shadow from a high point light
      glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 1.0f, 50.0f);
      glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      lightSpaceMatrix = lightProjection * lightView;

      gpu::GPU_shader_uniform_matrix4(shadow_shdr, "lightSpaceMatrix", &lightSpaceMatrix[0][0]);

      for (auto entity : objects_view) {
        auto &obj = registry.get<dna::Object>(entity);
        if (obj.type == dna::ObjectType::Mesh && obj.mesh) {
          auto model = glm::mat4(1.0f);
          model = glm::translate(model, obj.transform.location);
          model = glm::rotate(model, obj.transform.rotation.x, glm::vec3(1, 0, 0));
          model = glm::rotate(model, obj.transform.rotation.y, glm::vec3(0, 1, 0));
          model = glm::rotate(model, obj.transform.rotation.z, glm::vec3(0, 0, 1));
          model = glm::scale(model, obj.transform.scale);

          gpu::GPU_shader_uniform_matrix4(shadow_shdr, "model", &model[0][0]);

          static std::map<void *, gpu::GPUMesh *> shadow_mesh_cache;
          if (shadow_mesh_cache.find(obj.mesh.get()) == shadow_mesh_cache.end()) {
            shadow_mesh_cache[obj.mesh.get()] = gpu::GPU_mesh_create_from_dna_mesh(obj.mesh.get());
          }
          gpu::GPUMesh *gpu_mesh = shadow_mesh_cache[obj.mesh.get()];
          gpu::GPU_mesh_draw(gpu_mesh, nullptr);
        }
      }

      gpu::GPU_framebuffer_unbind();
      gl.glViewport(0, 0, width, height);
    }
  }

  // 3. Main Drawing Pass

  static gpu::GPUShader *g_shared_shader = nullptr;
  static bool g_shader_attempted = false;
  // Lazily create the shared program shader (requires active GPU context).
  // Only attempt once — if shader fails, do not spam retries every frame.
  auto ensure_shader = [&]() -> gpu::GPUShader * {
    if (!g_shared_shader && !g_shader_attempted) {
      g_shader_attempted = true;
      const std::string dir_path = std::string(lib::get_application_dir_path()) +
                                   "/../../source/runtime/gpu";
      gpu::GPUShaderSourceParameters params = {nullptr};
      if (is_opengl) {
        const std::string vert_path = dir_path + "/shaders/core/program/program.vert";
        const std::string frag_path = dir_path + "/shaders/core/program/program.frag";
        g_shared_shader = gpu::GPU_shader_create_from_source(
            vert_path.c_str(), frag_path.c_str(), &params);
      }
      else {
        const std::string metal_path = dir_path + "/shaders/core/program/program.metal";
        params.vert_entry = "vertex_main";
        params.frag_entry = "fragment_main";
        g_shared_shader = gpu::GPU_shader_create_from_source(
            metal_path.c_str(), metal_path.c_str(), &params);
      }
    }
    return g_shared_shader;
  };

  for (auto entity : objects_view) {
    auto &obj = registry.get<dna::Object>(entity);

    if ((obj.type == dna::ObjectType::Mesh || obj.type == dna::ObjectType::Light) && obj.mesh) {
      // Lazily assign shader if not yet created
      if (!obj.shader_program) {
        obj.shader_program = ensure_shader();
      }

      bool is_selected = (obj.select_flag & dna::BASE_SELECTED);
      auto *gpu_shader = (gpu::GPUShader *)obj.shader_program;

      if (gpu_shader) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, obj.transform.location);
        model = glm::rotate(model, obj.transform.rotation.x, glm::vec3(1, 0, 0));
        model = glm::rotate(model, obj.transform.rotation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, obj.transform.rotation.z, glm::vec3(0, 0, 1));
        model = glm::scale(model, obj.transform.scale);

        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec3 emissive = glm::vec3(0.0f);
        if (!obj.mesh->materials.empty() && obj.mesh->materials[0]) {
          color = obj.mesh->materials[0]->color.to_vec4();
          glm::vec4 em = obj.mesh->materials[0]->emissive_color.to_vec4();
          emissive = glm::vec3(em.x, em.y, em.z);
        }

        if (is_opengl) {
          gpu::GPU_shader_bind(gpu_shader);
          gpu::GPU_shader_uniform_matrix4(gpu_shader, "view", &view[0][0]);
          gpu::GPU_shader_uniform_matrix4(gpu_shader, "projection", &projection[0][0]);
          gpu::GPU_shader_uniform_matrix4(gpu_shader, "model", &model[0][0]);
          gpu::GPU_shader_uniform_matrix4(gpu_shader, "lightSpaceMatrix", &lightSpaceMatrix[0][0]);

          // Bind shadow map to texture slot 1
          auto *shadow_fb = get_shadow_fb();
          if (shadow_fb && shadow_fb->depth_tex) {
            QOpenGLFunctions_4_1_Core gl;
            gl.initializeOpenGLFunctions();
            gl.glActiveTexture(GL_TEXTURE1);
            gl.glBindTexture(GL_TEXTURE_2D, shadow_fb->depth_tex->opengl_id);
            gpu::GPU_shader_uniform_texture(gpu_shader, "shadowMap", 1);
          }

          float color_arr[3] = {color.x, color.y, color.z};
          gpu::GPU_shader_uniform_vector3(gpu_shader, "color", color_arr);
          float emissive_arr[3] = {emissive.x, emissive.y, emissive.z};
          gpu::GPU_shader_uniform_vector3(gpu_shader, "emissive", emissive_arr);

          gpu::GPU_shader_uniform_int(gpu_shader, "numLights", lighting.num_lights);
          for (int i = 0; i < lighting.num_lights; i++) {
            QString prefix = "lights[" + QString::number(i) + "].";
            gpu::GPU_shader_uniform_int(
                gpu_shader, (prefix + "type").toUtf8().constData(), lighting.lights[i].type);
            gpu::GPU_shader_uniform_vector3(gpu_shader,
                                            (prefix + "position").toUtf8().constData(),
                                            &lighting.lights[i].position.x);
            gpu::GPU_shader_uniform_vector3(
                gpu_shader, (prefix + "color").toUtf8().constData(), &lighting.lights[i].color.x);
            gpu::GPU_shader_uniform_float(
                gpu_shader, (prefix + "energy").toUtf8().constData(), lighting.lights[i].energy);
            gpu::GPU_shader_uniform_float(
                gpu_shader, (prefix + "range").toUtf8().constData(), lighting.lights[i].range);
          }
        }
        else {
          gpu::GPU_shader_bind_metal(gpu_shader, encoder_or_context);
          id<MTLRenderCommandEncoder> mtl_encoder = (id<MTLRenderCommandEncoder>)
              encoder_or_context;

          // Buffer(1): Uniforms (model, view, projection, lightSpaceMatrix)
          struct {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 lightSpaceMatrix;
          } uniforms;
          uniforms.model = model;
          uniforms.view = view;
          uniforms.projection = projection;
          uniforms.lightSpaceMatrix = lightSpaceMatrix;

          [mtl_encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];

          // Buffer(2): LightingUniforms
          [mtl_encoder setFragmentBytes:&lighting length:sizeof(lighting) atIndex:2];

          // Buffer(0): Color (fragment)
          [mtl_encoder setFragmentBytes:&color length:sizeof(color) atIndex:0];

          // Buffer(3): Emissive (fragment)
          glm::vec4 emissive4 = glm::vec4(emissive, 0.0f);
          [mtl_encoder setFragmentBytes:&emissive4 length:sizeof(emissive4) atIndex:3];
        }

        static std::map<void *, gpu::GPUMesh *> mesh_cache;
        if (mesh_cache.find(obj.mesh.get()) == mesh_cache.end()) {
          mesh_cache[obj.mesh.get()] = gpu::GPU_mesh_create_from_dna_mesh(obj.mesh.get());
        }
        gpu::GPUMesh *gpu_mesh = mesh_cache[obj.mesh.get()];

        QOpenGLFunctions_4_1_Core gl;
        gl.initializeOpenGLFunctions();
        if (is_selected) {
          gl.glEnable(GL_STENCIL_TEST);
          gl.glClear(GL_STENCIL_BUFFER_BIT);
          gl.glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
          gl.glStencilFunc(GL_ALWAYS, 1, 0xFF);
          gl.glStencilMask(0xFF);
        }

        gpu::GPU_mesh_draw(gpu_mesh, encoder_or_context);

        if (is_selected) {
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

}  // namespace vektor::draw
