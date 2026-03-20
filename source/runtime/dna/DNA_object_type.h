#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "../kernel/VKE_object_component.hh"
#include "DNA_camera.h"
#include "DNA_id.h"
#include "DNA_light.h"
#include "DNA_mesh_types.h"

namespace vektor::dna {

struct Selected {
  bool selected;
};

struct Active {
  bool active;
};

enum class ObjectType : uint8_t { Mesh, Camera, Light, Empty };

typedef struct Transform {
  glm::vec3 location = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
} Transform;

enum OBSelectionFlags {
  BASE_SELECTED = (1 << 0),
  BASE_ACTIVE = (1 << 1),
};

typedef struct Object {
  ID id;
  char description[256] = "";
  Transform transform;
  void *shader_program = nullptr;

  uint32_t select_flag = 0;

  std::shared_ptr<dna::Mesh> mesh;
  std::shared_ptr<dna::DNA_Light> light;
  std::shared_ptr<dna::DNA_Camera> camera;

  ObjectType type = ObjectType::Empty;

  std::vector<std::shared_ptr<kernel::VKE_ObjectComponent>> components;

  Object() = default;
} Object;

// Dummy components for future use
class PhysicsComponent : public kernel::VKE_ObjectComponent {
 public:
  void on_transform_updated() override
  {
    // Physics sync logic
  }
};

}  // namespace vektor::dna
