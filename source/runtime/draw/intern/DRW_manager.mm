#include <QDir>
#include <QOpenGLFunctions_4_1_Core>
#include <QString>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>

#include "../../../intern/clog/CLG_log.h"
#include "../../creator_global.h"
#include "../../kernel/ecs/ECS_registry.h"
#include "../../lib/intern/appdir.h"
#include "../DRW_manager.hh"
#include "../gpu/GPU_framebuffer.h"
#include "../gpu/GPU_shader.h"
#include "../gpu/GPU_vertex_buffer.hh"

#ifdef __APPLE__
#  include "../../../../intern/vpi/intern/VPI_ContextMTL.hh"
#endif

namespace vektor::draw {

CLG_LOGREF_DECLARE_GLOBAL(LOG_DRAW, "draw");

// Align with Metal/GLSL layout (64 bytes)
struct GPULight {
  int type;
  float _pad0[3];
  glm::vec3 position;
  float _pad1;
  glm::vec3 color;
  float _pad2;
  float energy;
  float range;
  float _pad3[2];
};

#define MAX_LIGHTS 8
struct LightingUniforms {
  int num_lights;
  float _pad0[3];
  GPULight lights[MAX_LIGHTS];
};

#define MAX_SHADOW_LIGHTS 8

static gpu::GPUShader *get_shadow_shader()
{
  static gpu::GPUShader *shdr = nullptr;
  if (!shdr) {
    const bool is_opengl = (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL);
    QString shader_path = QString(vektor::lib::get_application_dir_path()) +
                          "/../../source/runtime/gpu/shaders/core/shadow/shadow";
    if (is_opengl) {
      shdr = gpu::GPU_shader_create_from_source((shader_path + ".vert").toUtf8().constData(),
                                                (shader_path + ".frag").toUtf8().constData());
    }
    else {
      gpu::GPUShaderSourceParameters params = {"vertex_main", "fragment_main"};
      shdr = gpu::GPU_shader_create_from_source((shader_path + ".metal").toUtf8().constData(),
                                                (shader_path + ".metal").toUtf8().constData(),
                                                &params);
    }
  }
  return shdr;
}

static gpu::GPUFrameBuffer *get_shadow_fb_array()
{
  static gpu::GPUFrameBuffer *fb = nullptr;
  if (!fb) {
    fb = gpu::GPU_framebuffer_create_depth_array(2048, 2048, MAX_SHADOW_LIGHTS);
  }
  return fb;
}

static LightingUniforms g_lighting = {};
static glm::mat4 g_lightSpaceMatrices[MAX_SHADOW_LIGHTS];

void DRW_prepare_view(vektor::dna::Scene *scene)
{
  auto &registry = kernel::ECSRegistry::instance().registry();
  auto objects_view = registry.view<dna::Object>();

  // 1. Gather active lights in the scene
  g_lighting.num_lights = 0;
  for (auto entity : objects_view) {
    auto &l_obj = registry.get<dna::Object>(entity);
    if (l_obj.type == dna::ObjectType::Light && l_obj.light && g_lighting.num_lights < MAX_LIGHTS)
    {
      auto &la = *l_obj.light;
      GPULight &gl = g_lighting.lights[g_lighting.num_lights++];
      gl.type = (int)la.type;
      gl.position = l_obj.transform.location;
      gl.color = la.color;
      gl.energy = la.energy;
      gl.range = la.distance;
    }
  }

  // 2. Shadow Pass
  const bool is_opengl = (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL);
  for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
    g_lightSpaceMatrices[i] = glm::mat4(1.0f);
  }

  if (g_lighting.num_lights > 0) {
    auto *shadow_shdr = get_shadow_shader();
    auto *shadow_fb = get_shadow_fb_array();

    if (shadow_shdr && shadow_fb) {
      for (int i = 0; i < std::min(g_lighting.num_lights, MAX_SHADOW_LIGHTS); i++) {
        glm::vec3 lightPos = g_lighting.lights[i].position;
        glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 1.0f, 50.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        g_lightSpaceMatrices[i] = lightProjection * lightView;

        if (is_opengl) {
          gpu::GPU_framebuffer_attach_depth_layer(shadow_fb, i);
          QOpenGLFunctions_4_1_Core gl_func;
          gl_func.initializeOpenGLFunctions();
          gl_func.glViewport(0, 0, shadow_fb->width, shadow_fb->height);
          gl_func.glClear(GL_DEPTH_BUFFER_BIT);

          gpu::GPU_shader_bind(shadow_shdr);
          gpu::GPU_shader_uniform_matrix4(
              shadow_shdr, "lightSpaceMatrix", &g_lightSpaceMatrices[i][0][0]);

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
                shadow_mesh_cache[obj.mesh.get()] = gpu::GPU_mesh_create_from_dna_mesh(
                    obj.mesh.get());
              }
              gpu::GPUMesh *gpu_mesh = shadow_mesh_cache[obj.mesh.get()];
              gpu::GPU_mesh_draw(gpu_mesh, nullptr);
            }
          }
          gpu::GPU_framebuffer_unbind();
        }
        else {
#ifdef __APPLE__
          auto *vpi_context = ::vpi::VPI_Context::get_active_context();
          if (vpi_context) {
            auto *mtl_context = dynamic_cast<::vpi::VPI_ContextMTL *>(vpi_context);
            if (mtl_context) {
              id<MTLCommandBuffer> commandBuffer = (id<MTLCommandBuffer>)
                                                       mtl_context->get_current_command_buffer();
              bool owns_command_buffer = false;
              if (!commandBuffer) {
                commandBuffer = [mtl_context->get_metal_command_queue() commandBuffer];
                owns_command_buffer = true;
              }

              if (commandBuffer) {
                MTLRenderPassDescriptor *passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
                passDesc.depthAttachment.texture = (id<MTLTexture>)
                                                       shadow_fb->depth_tex->metal_texture;
                passDesc.depthAttachment.slice = i;
                passDesc.depthAttachment.loadAction = MTLLoadActionClear;
                passDesc.depthAttachment.storeAction = MTLStoreActionStore;
                passDesc.depthAttachment.clearDepth = 1.0;

                id<MTLRenderCommandEncoder> shadowEncoder = [commandBuffer
                    renderCommandEncoderWithDescriptor:passDesc];
                [shadowEncoder setLabel:[NSString stringWithFormat:@"ShadowPass_Light%d", i]];
                gpu::GPU_shader_bind_metal(shadow_shdr, shadowEncoder);

                for (auto entity : objects_view) {
                  auto &obj = registry.get<dna::Object>(entity);
                  if (obj.type == dna::ObjectType::Mesh && obj.mesh) {
                    auto model = glm::mat4(1.0f);
                    model = glm::translate(model, obj.transform.location);
                    model = glm::rotate(model, obj.transform.rotation.x, glm::vec3(1, 0, 0));
                    model = glm::rotate(model, obj.transform.rotation.y, glm::vec3(0, 1, 0));
                    model = glm::rotate(model, obj.transform.rotation.z, glm::vec3(0, 0, 1));
                    model = glm::scale(model, obj.transform.scale);

                    struct {
                      glm::mat4 model;
                      glm::mat4 lightSpaceMatrix;
                    } uniforms = {};
                    uniforms.model = model;
                    uniforms.lightSpaceMatrix = g_lightSpaceMatrices[i];

                    [shadowEncoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];

                    static std::map<void *, gpu::GPUMesh *> shadow_mesh_cache;
                    if (shadow_mesh_cache.find(obj.mesh.get()) == shadow_mesh_cache.end()) {
                      shadow_mesh_cache[obj.mesh.get()] = gpu::GPU_mesh_create_from_dna_mesh(
                          obj.mesh.get());
                    }
                    gpu::GPUMesh *gpu_mesh = shadow_mesh_cache[obj.mesh.get()];
                    gpu::GPU_mesh_draw(gpu_mesh, shadowEncoder);
                  }
                }
                [shadowEncoder endEncoding];
                if (owns_command_buffer) {
                  [commandBuffer commit];
                }
              }
            }
          }
#endif
        }
      }
    }
  }
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

  static gpu::GPUShader *gpu_shader = nullptr;
  static bool shader_failed = false;

  if (shader_failed)
    return;

  if (!gpu_shader) {
    QString shader_base = QString(vektor::lib::get_application_dir_path()) +
                          "/../../source/runtime/gpu/shaders/core/program/program";
    if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
      gpu_shader = gpu::GPU_shader_create_from_source(
          (shader_base + ".vert").toUtf8().constData(),
          (shader_base + ".frag").toUtf8().constData());
    }
    else {
      gpu_shader = gpu::GPU_shader_create_from_source(
          (shader_base + ".metal").toUtf8().constData(),
          (shader_base + ".metal").toUtf8().constData());
    }
    if (!gpu_shader) {
      shader_failed = true;
      CLOG_ERROR(LOG_DRAW, "Failed to create main shader.");
      return;
    }
  }

  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl_func;
    gl_func.initializeOpenGLFunctions();
    // CRITICAL: Restore viewport for the main scene draw
    gl_func.glViewport(0, 0, width, height);

    gpu::GPU_shader_bind(gpu_shader);
    gpu::GPU_shader_uniform_matrix4(gpu_shader, "view", &view[0][0]);
    gpu::GPU_shader_uniform_matrix4(gpu_shader, "projection", &projection[0][0]);
    gpu::GPU_shader_uniform_float(gpu_shader, "time", time);

    gpu::GPU_shader_uniform_matrix4_array(
        gpu_shader, "lightSpaceMatrices", &g_lightSpaceMatrices[0][0][0], MAX_SHADOW_LIGHTS);

    auto *shadow_fb = get_shadow_fb_array();
    if (shadow_fb && shadow_fb->depth_tex) {
      gl_func.glActiveTexture(GL_TEXTURE1);
      gl_func.glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_fb->depth_tex->opengl_id);
      gpu::GPU_shader_uniform_texture(gpu_shader, "shadowMapArray", 1);
    }

    gpu::GPU_shader_uniform_int(gpu_shader, "numLights", g_lighting.num_lights);
    for (int i = 0; i < g_lighting.num_lights; i++) {
      QString prefix = QString("lights[%1].").arg(i);
      gpu::GPU_shader_uniform_int(
          gpu_shader, (prefix + "type").toUtf8().constData(), g_lighting.lights[i].type);
      gpu::GPU_shader_uniform_vector3(gpu_shader,
                                      (prefix + "position").toUtf8().constData(),
                                      &g_lighting.lights[i].position[0]);
      gpu::GPU_shader_uniform_vector3(
          gpu_shader, (prefix + "color").toUtf8().constData(), &g_lighting.lights[i].color[0]);
      gpu::GPU_shader_uniform_float(
          gpu_shader, (prefix + "energy").toUtf8().constData(), g_lighting.lights[i].energy);
      gpu::GPU_shader_uniform_float(
          gpu_shader, (prefix + "range").toUtf8().constData(), g_lighting.lights[i].range);
    }

    // Draw objects (Both meshes and light icons)
    for (auto entity : objects_view) {
      auto &obj = registry.get<dna::Object>(entity);
      if ((obj.type == dna::ObjectType::Mesh || obj.type == dna::ObjectType::Light) && obj.mesh) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, obj.transform.location);
        model = glm::rotate(model, obj.transform.rotation.x, glm::vec3(1, 0, 0));
        model = glm::rotate(model, obj.transform.rotation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, obj.transform.rotation.z, glm::vec3(0, 0, 1));
        model = glm::scale(model, obj.transform.scale);
        gpu::GPU_shader_uniform_matrix4(gpu_shader, "model", &model[0][0]);
        gpu::GPU_shader_uniform_int(gpu_shader, "isLight", obj.type == dna::ObjectType::Light);

        dna::Color objectColor = {0.8f, 0.8f, 0.8f, 1.0f};
        if (obj.mesh && !obj.mesh->materials.empty()) {
          objectColor = obj.mesh->materials[0]->color;
        }
        float color_val[4] = {objectColor.r, objectColor.g, objectColor.b, objectColor.a};
        gpu::GPU_shader_uniform_vector4(gpu_shader, "objectColor", color_val);

        static std::map<void *, gpu::GPUMesh *> mesh_cache;
        if (mesh_cache.find(obj.mesh.get()) == mesh_cache.end()) {
          mesh_cache[obj.mesh.get()] = gpu::GPU_mesh_create_from_dna_mesh(obj.mesh.get());
        }
        gpu::GPUMesh *gpu_mesh = mesh_cache[obj.mesh.get()];
        gpu::GPU_mesh_draw(gpu_mesh, nullptr);
      }
    }
  }
  else {
#ifdef __APPLE__
    id<MTLRenderCommandEncoder> mtl_encoder = (id<MTLRenderCommandEncoder>)encoder_or_context;
    if (!mtl_encoder)
      return;

    gpu::GPU_shader_bind_metal(gpu_shader, mtl_encoder);

    // Set depth stencil state for Metal
    auto device = (id<MTLDevice>)vpi::VPI_ContextMTL::get_current_device();
    MTLDepthStencilDescriptor *depthDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthDesc.depthWriteEnabled = YES;
    id<MTLDepthStencilState> depthStencilState = [device
        newDepthStencilStateWithDescriptor:depthDesc];
    [mtl_encoder setDepthStencilState:depthStencilState];

    struct {
      glm::mat4 view;
      glm::mat4 projection;
      glm::mat4 lightSpaceMatrices[MAX_SHADOW_LIGHTS];
      LightingUniforms lighting;
      glm::vec3 viewPos;
      float _pad_viewPos;
      float time;
      float padding[3];
    } uniforms = {};

    uniforms.view = view;
    uniforms.projection = projection;
    uniforms.viewPos = glm::vec3(glm::inverse(view)[3]);  // Camera position
    for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
      uniforms.lightSpaceMatrices[i] = g_lightSpaceMatrices[i];
    }
    uniforms.lighting = g_lighting;
    uniforms.time = time;

    [mtl_encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];
    [mtl_encoder setFragmentBytes:&uniforms length:sizeof(uniforms) atIndex:1];

    auto *shadow_fb = get_shadow_fb_array();
    if (shadow_fb && shadow_fb->depth_tex) {
      [mtl_encoder setFragmentTexture:(id<MTLTexture>)shadow_fb->depth_tex->metal_texture
                              atIndex:0];
    }

    for (auto entity : objects_view) {
      auto &obj = registry.get<dna::Object>(entity);
      if ((obj.type == dna::ObjectType::Mesh || obj.type == dna::ObjectType::Light) && obj.mesh) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, obj.transform.location);
        model = glm::rotate(model, obj.transform.rotation.x, glm::vec3(1, 0, 0));
        model = glm::rotate(model, obj.transform.rotation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, obj.transform.rotation.z, glm::vec3(0, 0, 1));
        model = glm::scale(model, obj.transform.scale);

        struct {
          glm::mat4 model;
          int isLight;
          float padding[3];
        } obj_uniforms = {model, (obj.type == dna::ObjectType::Light), {0, 0, 0}};

        [mtl_encoder setVertexBytes:&obj_uniforms length:sizeof(obj_uniforms) atIndex:2];

        // Restore missing fragment buffers for color and emissive
        dna::Color color = {0.8f, 0.8f, 0.8f, 1.0f};
        glm::vec3 emissive = {0.0f, 0.0f, 0.0f};
        if (!obj.mesh->materials.empty()) {
          color = obj.mesh->materials[0]->color;
          emissive = glm::vec3(obj.mesh->materials[0]->emissive_color.r,
                               obj.mesh->materials[0]->emissive_color.g,
                               obj.mesh->materials[0]->emissive_color.b);
        }
        [mtl_encoder setFragmentBytes:&color length:sizeof(color) atIndex:0];
        [mtl_encoder setFragmentBytes:&emissive length:sizeof(emissive) atIndex:3];

        static std::map<void *, gpu::GPUMesh *> mesh_cache;
        if (mesh_cache.find(obj.mesh.get()) == mesh_cache.end()) {
          mesh_cache[obj.mesh.get()] = gpu::GPU_mesh_create_from_dna_mesh(obj.mesh.get());
        }
        gpu::GPUMesh *gpu_mesh = mesh_cache[obj.mesh.get()];
        gpu::GPU_mesh_draw(gpu_mesh, mtl_encoder);
      }
    }
#endif
  }
}

}  // namespace vektor::draw
