#pragma once

#include <glm/glm.hpp>

#include "DNA_id_enum.h"

namespace vektor::dna {

typedef enum {
  CAM_PERSP = 0,
  CAM_ORTHO = 1,
  CAM_PANO = 2,
  CAM_CUSTOM = 3,
} DNA_CameraType;

struct DNA_Camera {
  static constexpr ID_Type id_type = ID_CA;

  DNA_CameraType type;

  glm::vec3 pivot = glm::vec3(0.0f);
  float distance = 15.0f;
  float rotation_x = 30.0f;
  float rotation_y = 45.0f;
  float fov = 45.0f;
  float near_plane = 0.1f;
  float far_plane = 1000.0f;
  float sensor_width = 36.0f;
  float sensor_height = 24.0f;
};

}  // namespace vektor::dna
