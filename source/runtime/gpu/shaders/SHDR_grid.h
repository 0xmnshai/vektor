#ifndef SHDR_GRID_H
#define SHDR_GRID_H

#include "SHDR_base.h"
#include <QOpenGLFunctions_4_1_Core>

namespace vektor::gpu {

class GridShader : public BaseShader, protected QOpenGLFunctions_4_1_Core {
 public:
  GridShader();
  virtual ~GridShader();

  void init(const QString &shader_path) override;
  void draw(const glm::mat4 &projection, const glm::mat4 &view) override;

 private:
  GLuint m_vao = 0;
  GLuint m_vbo = 0;
  void *m_mtl_vbo = nullptr;
};

}  // namespace vektor::gpu

#endif

/*
OPENGL WITHOUT SLANG
#ifndef SHDR_GRID_H
#define SHDR_GRID_H

#include "SHDR_base.h"
#include <QOpenGLFunctions_4_1_Core>
#include <QMatrix4x4>

namespace vektor::gpu {

class GridShader : public BaseShader, protected QOpenGLFunctions_4_1_Core {
 public:
  GridShader() : BaseShader("Grid") {}
  virtual ~GridShader()
  {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
  }

  void init(const QString &shader_path) override
  {
    QString vert_file = shader_path + "/grid.vert.slang";
    QString frag_file = shader_path + "/grid.frag.slang";

    shader_ = GPU_shader_create_from_slang(vert_file.toUtf8().constData(),
                                           frag_file.toUtf8().constData());

    if (!shader_) {
        qDebug() << "Failed to create Grid shader from Slang";
        return;
    }

    if (shader_->backend == GPUShader::GPU_BACKEND_OPENGL) {
        initializeOpenGLFunctions();
        
        // Full screen quad for grid
        float vertices[] = {-1.0f, -1.0f, 0.0f,
                             1.0f, -1.0f, 0.0f,
                            -1.0f,  1.0f, 0.0f,
                            -1.0f,  1.0f, 0.0f,
                             1.0f, -1.0f, 0.0f,
                             1.0f,  1.0f, 0.0f};

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        QOpenGLShaderProgram *program = GPU_shader_get_program(shader_);
        int posAttr = program->attributeLocation("aPos");
        glEnableVertexAttribArray(posAttr);
        glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    else {
        // Metal initialization would go here (buffers etc)
        qDebug() << "GridShader: Metal backend initialization pending buffer implementation";
    }
  }

  void draw(const glm::mat4 &projection, const glm::mat4 &view) override
  {
    if (!shader_)
      return;

    if (shader_->backend == GPUShader::GPU_BACKEND_OPENGL) {
        QOpenGLShaderProgram *program = GPU_shader_get_program(shader_);
        if (!program || !program->isLinked()) return;

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
        // Metal draw implementation
    }
  }

 private:
  GLuint m_vao = 0;
  GLuint m_vbo = 0;
  // Metal members ...
};

}  // namespace vektor::gpu

#endif


*/