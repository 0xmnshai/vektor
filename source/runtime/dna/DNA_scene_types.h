#pragma once

#include "DNA_id.h"
#include "DNA_object_type.h"

namespace vektor::dna {


  // modify the scene according to the codebase ? 
typedef struct Scene {
  ID id;

  // For now, simpler than a full collection system
  Object **objects;
  int objects_len;

  void *world;
} Scene;

}  // namespace vektor::dna
