#pragma once

#include <glm/glm.hpp>

namespace vektor::dna {
struct Uniforms {
  glm::mat4 model;
  glm::mat4 view;        // Supplied by viewport later
  glm::mat4 projection;  // Supplied by viewport later
};
}  // namespace vektor::dna
