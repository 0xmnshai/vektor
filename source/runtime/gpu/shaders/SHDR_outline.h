#ifndef SHDR_OUTLINE_H
#define SHDR_OUTLINE_H

#include "SHDR_base.h"
#include <QOpenGLFunctions_4_1_Core>

namespace vektor::gpu {

class OutlineShader : public BaseShader, protected QOpenGLFunctions_4_1_Core {
 public:
  OutlineShader();
  virtual ~OutlineShader();

  virtual void init(const QString &shaderDir) override;
  virtual void draw(const glm::mat4 &projection, const glm::mat4 &view) override;
  void draw(const glm::mat4 &projection,
            const glm::mat4 &view,
            const glm::mat4 &model,
            int width,
            int height,
            float time);

 private:
  void *m_mtl_vbo = nullptr;
};

}  // namespace vektor::gpu

#endif
