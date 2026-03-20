#pragma once

#include "DNA_id_enum.h"
#include <glm/glm.hpp>

namespace vektor::dna {

typedef enum DNA_LightType {
  LA_LOCAL = 0,       /* Point */
} DNA_LightType;

struct DNA_Light {
  static constexpr ID_Type id_type = ID_LA;

  DNA_LightType type;
  glm::vec3 color = glm::vec3(1.0f);
  float energy = 1.0f;
  float temperature = 6500.0f; /* D65 / Daylight */
  float specular_factor = 1.0f;
  float diffuse_factor = 1.0f;

  float distance = 10.0f;   /* for point light falloff */
};

}  // namespace vektor::dna
