#pragma once

#include "DNA_object_type.h"

namespace vektor::dna {

typedef struct Scene {
  char id_name[64];

  // For now, simpler than a full collection system
  Object **objects;
  int objects_len;

  void *world;
} Scene;

}  // namespace vektor::dna
