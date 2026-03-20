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

  // we will use this for creating default scene and then a scene ( Mesh )  editor to manage scene
  // entities ...

  // Create test entities only once
  vektor::kernel::create_entity(nullptr,
                                nullptr,
                                "Plane",
                                "Contruct a Plane for default scene view",
                                (int)vektor::dna::ObjectType::Mesh,
                                0.0f,
                                0.0f,
                                0.0f,
                                0.26f,
                                0.27f,
                                0.29f);
}

}  // namespace qt::scene
