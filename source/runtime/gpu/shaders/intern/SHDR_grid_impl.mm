#import <Metal/Metal.h>
#include <QDebug>
#include <QMatrix4x4>
#import <QuartzCore/QuartzCore.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../../../intern/vpi/intern/VPI_ContextMTL.hh"
#include "../../../runtime/creator_global.h"
#include "../SHDR_grid.h"


// TODO: Implement shader class for every object
// TODO: Create some pre-defined object shaders so that it's easy to use 

namespace vektor::gpu {

GridShader::GridShader() : BaseShader("Grid") {}

GridShader::~GridShader()
{
  if (m_vao) {
    initializeOpenGLFunctions();
    glDeleteVertexArrays(1, &m_vao);
  }
  if (m_vbo) {
    initializeOpenGLFunctions();
    glDeleteBuffers(1, &m_vbo);
  }
#ifdef __APPLE__
  if (m_mtl_vbo) {
    [(id<MTLBuffer>)m_mtl_vbo release];
    m_mtl_vbo = nullptr;
  }
#endif
}

void GridShader::init(const QString &shader_path)
{
  QString vert_file = shader_path + "/grid.vert";
  QString frag_file = shader_path + "/grid.frag";

#ifdef __APPLE__
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    vert_file = shader_path + "/grid.metal";
  }
#endif

  shader_ = GPU_shader_create_from_source(vert_file.toUtf8().constData(),
                                          frag_file.toUtf8().constData());

  if (!shader_) {
    qDebug() << "Failed to create Grid shader from source";
    return;
  }

  if (shader_->backend == GPUShader::GPU_BACKEND_OPENGL) {
    initializeOpenGLFunctions();

    // Full screen quad for grid
    float vertices[] = {-1.0f,
                        -1.0f,
                        0.0f,
                        1.0f,
                        -1.0f,
                        0.0f,
                        -1.0f,
                        1.0f,
                        0.0f,
                        -1.0f,
                        1.0f,
                        0.0f,
                        1.0f,
                        -1.0f,
                        0.0f,
                        1.0f,
                        1.0f,
                        0.0f};

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    QOpenGLShaderProgram *program = GPU_shader_get_program(shader_);
    int pos_attr = program->attributeLocation("aPos");
    glEnableVertexAttribArray(pos_attr);
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  else {
#ifdef __APPLE__
    auto device = (id<MTLDevice>)vpi::VPI_ContextMTL::get_current_device();
    if (device) {
      float vertices[] = {-1.0f,
                          -1.0f,
                          0.0f,
                          1.0f,
                          -1.0f,
                          0.0f,
                          -1.0f,
                          1.0f,
                          0.0f,
                          -1.0f,
                          1.0f,
                          0.0f,
                          1.0f,
                          -1.0f,
                          0.0f,
                          1.0f,
                          1.0f,
                          0.0f};
      id<MTLBuffer> buffer = [device newBufferWithBytes:vertices
                                                 length:sizeof(vertices)
                                                options:MTLResourceStorageModeShared];
      m_mtl_vbo = (void *)buffer;
    }
#endif
  }
}

void GridShader::draw(const glm::mat4 &projection, const glm::mat4 &view)
{
  if (!shader_)
    return;

  if (shader_->backend == GPUShader::GPU_BACKEND_OPENGL) {
    QOpenGLShaderProgram *program = GPU_shader_get_program(shader_);
    if (!program || !program->isLinked())
      return;

    program->bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glm::mat4 viewInv = glm::inverse(view);
    glm::mat4 projInv = glm::inverse(projection);

    program->setUniformValue("projection", QMatrix4x4(&projection[0][0]).transposed());
    program->setUniformValue("view", QMatrix4x4(&view[0][0]).transposed());
    program->setUniformValue("viewInverse", QMatrix4x4(&viewInv[0][0]).transposed());
    program->setUniformValue("projInverse", QMatrix4x4(&projInv[0][0]).transposed());

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    program->release();
  }
  else {
#ifdef __APPLE__
    auto *active_context = vpi::VPI_ContextMTL::get_active_context();
    auto *mtl_context = dynamic_cast<vpi::VPI_ContextMTL *>(active_context);

    if (mtl_context) {
      mtl_context->set_depth_write_enabled(false);
      auto encoder = (id<MTLRenderCommandEncoder>)mtl_context->get_current_command_encoder();

      if (encoder && shader_->metal_pipeline) {
        auto pipeline = (id<MTLRenderPipelineState>)shader_->metal_pipeline;
        auto vbo = (id<MTLBuffer>)m_mtl_vbo;

        if (!vbo) {
          return;
        }

        [encoder setRenderPipelineState:pipeline];
        [encoder setVertexBuffer:vbo offset:0 atIndex:0];

        typedef struct uniforms {
          glm::mat4 projection;
          glm::mat4 view;
          glm::mat4 viewInverse;
          glm::mat4 projInverse;
        } Uniforms;

        Uniforms uniforms;
        uniforms.projection = projection;
        uniforms.view = view;
        uniforms.viewInverse = glm::inverse(view);
        uniforms.projInverse = glm::inverse(projection);

        [encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];
        [encoder setFragmentBytes:&uniforms length:sizeof(uniforms) atIndex:1];

        [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];
      }
      mtl_context->set_depth_write_enabled(true);
    }
#endif
  }
}

}  // namespace vektor::gpu
