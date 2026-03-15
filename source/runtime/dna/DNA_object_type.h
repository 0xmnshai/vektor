#pragma once

#include <glm/glm.hpp>

namespace vektor::dna {

// we can add more as needed
typedef enum EntityTypeDNA {
  DNA_ENTITY_CUBE = 0,
  DNA_ENTITY_CYLINDER = 1,
  DNA_ENTITY_PLANE = 2,
  DNA_ENTITY_SPHERE = 3,
} EntityTypeDNA;

typedef enum ObjectTypeDNA {
  DNA_COLLECTION = 0,
  DNA_MESH = 1,
  DNA_CAMERA = 2,
  DNA_LIGHT = 3,
} ObjectTypeDNA;

typedef struct Transform {
  glm::vec3 location;
  glm::vec3 scale;
  glm::vec3 rotation;
} Transform;

typedef struct Material {
  glm::vec4 color;
} Material;

typedef struct Object {
  char id_name[64];
  Transform transform;
  EntityTypeDNA type;
  Material material;
  void *shader_program;
  ObjectTypeDNA object_type;
} Object;

}  // namespace vektor::dna
