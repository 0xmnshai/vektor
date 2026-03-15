#pragma once

#include <QString>
#include <glm/glm.hpp>

#include "../GPU_shader.h"

namespace vektor::gpu {
class BaseShader {
 public:
  explicit BaseShader(QString name) : name_(std::move(name)), shader_(nullptr) {}
  virtual ~BaseShader()
  {
    if (shader_) {
      GPU_shader_free(shader_);
    }
  }

  virtual void init(const QString &shaderDir) = 0;
  virtual void draw(const glm::mat4 &projection, const glm::mat4 &view) = 0;

  [[nodiscard]] const QString &get_name() const
  {
    return name_;
  }

  [[nodiscard]] GPUShader *get_shader()
  {
    return shader_;
  }

 protected:
  const QString name_;
  GPUShader *shader_;
};
}  // namespace vektor::gpu
