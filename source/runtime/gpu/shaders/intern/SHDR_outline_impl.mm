#import <Metal/Metal.h>
#include <QDebug>
#include <QMatrix4x4>
#import <QuartzCore/QuartzCore.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../../../intern/vpi/intern/VPI_ContextMTL.hh"
#include "../../../runtime/creator_global.h"
#include "../../dna/DNA_uniform.h"
#include "../SHDR_outline.h"

namespace vektor::gpu {

OutlineShader::OutlineShader() : BaseShader("Outline") {}

OutlineShader::~OutlineShader() {}

void OutlineShader::init(const QString &shaderDir)
{
  QString vert_file = shaderDir + "/outline/outline.vert";
  QString frag_file = shaderDir + "/outline/outline.frag";

#ifdef __APPLE__
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    vert_file = shaderDir + "/outline/outline.metal";
  }
#endif

  shader_ = GPU_shader_create_from_source(vert_file.toUtf8().constData(),
                                          frag_file.toUtf8().constData());

  if (!shader_) {
    qDebug() << "Failed to create Outline shader from source";
    return;
  }
}

void OutlineShader::draw(const glm::mat4 &projection, const glm::mat4 &view)
{
  (void)projection;
  (void)view;
}

void OutlineShader::draw(const glm::mat4 &projection,
                         const glm::mat4 &view,
                         const glm::mat4 &model,
                         int width,
                         int height,
                         float time)
{
  if (!shader_)
    return;

  if (shader_->backend == GPUShader::GPU_BACKEND_OPENGL) {
    QOpenGLShaderProgram *program = GPU_shader_get_program(shader_);
    if (!program || !program->isLinked())
      return;

    program->bind();

    program->setUniformValue("projection", QMatrix4x4(&projection[0][0]).transposed());
    program->setUniformValue("view", QMatrix4x4(&view[0][0]).transposed());
    program->setUniformValue("model", QMatrix4x4(&model[0][0]).transposed());
    program->setUniformValue("u_viewportSize", (float)width, (float)height);
    program->setUniformValue("u_time", time);
    program->setUniformValue("offset", 0.05f); // Still pass offset for legacy or base thickness
  }
  else {
#ifdef __APPLE__
    auto *active_context = vpi::VPI_ContextMTL::get_active_context();
    auto *mtl_context = dynamic_cast<vpi::VPI_ContextMTL *>(active_context);

    if (mtl_context) {
      auto encoder = (id<MTLRenderCommandEncoder>)mtl_context->get_current_command_encoder();

      if (encoder && shader_->metal_pipeline) {
        auto pipeline = (id<MTLRenderPipelineState>)shader_->metal_pipeline;

        [encoder setRenderPipelineState:pipeline];

        dna::Uniforms uniforms = {};
        uniforms.model = model;
        uniforms.view = view;
        uniforms.projection = projection;
        uniforms.u_viewportSize = glm::vec2((float)width, (float)height);
        uniforms.u_time = time;
        uniforms.offset = 0.02f;

        [encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];
      }
    }
#endif
  }
}

}  // namespace vektor::gpu
