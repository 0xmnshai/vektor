#pragma once 

#include <glm/glm.hpp>

namespace vektor::dna {

// Helper struct to pack data for GPU
struct GPUVertex {
  glm::vec3 pos;
  glm::vec3 normal;
};

}