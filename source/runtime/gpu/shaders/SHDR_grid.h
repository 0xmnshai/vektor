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