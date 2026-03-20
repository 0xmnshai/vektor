#include "SCN_setup.h"
#include "../../../../../source/runtime/dna/DNA_object_type.h"
#include "../../../../../source/runtime/kernel/ecs/ECS_registry.h"

namespace qt::scene {

void SCN_init_default_scene()
{
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true; 

  // Create a Cube entity
  vektor::kernel::create_entity(nullptr,
                                nullptr,
                                "Cube",
                                "Contruct a Cube for default scene view",
                                (int)vektor::dna::ObjectType::Mesh,
                                0.0f,
                                1.0f,
                                0.0f,
                                0.26f,
                                0.27f,
                                0.29f); // Default gray cube

  // Create a Point light entity
  vektor::kernel::create_entity(nullptr,
                                nullptr,
                                "Point",
                                "Contruct a Point light for default scene view",
                                (int)vektor::dna::ObjectType::Light,
                                5.0f,
                                10.0f,
                                5.0f,
                                1.0f,
                                1.0f,
                                1.0f);
  
  // Brighten the default point light
  auto &registry = vektor::kernel::ECSRegistry::instance().registry();
  auto view = registry.view<vektor::dna::Object>();
  for (auto entity : view) {
    auto &obj = view.get<vektor::dna::Object>(entity);
    if (std::string(obj.id.name) == "Point" && obj.light) {
        obj.light->energy = 10.0f;
        obj.light->distance = 25.0f;
        break;
    }
  }
}

}  // namespace qt::scene
