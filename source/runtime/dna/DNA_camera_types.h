#pragma once

#include "DNA_id_enum.h"

namespace vektor::dna {

#define DEFAULT_CAMAERA_SENSOR_WIDTH 36.0f
#define DEFAULT_CAMAERA_SENSOR_HEIGHT 24.0f

typedef enum {
  CAM_PERSP = 0,
  CAM_ORTHO = 1,
  CAM_PANO = 2,
  CAM_CUSTOM = 3,
} DNA_CameraType;

struct Camera {
  static constexpr ID_Type id_type = ID_CA;

  DNA_CameraType type;

  // clip ? sensor size ? length ?

};

}  // namespace vektor::dna
