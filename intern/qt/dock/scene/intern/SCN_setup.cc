#include "SCN_setup.h"
#include "../../../../../source/runtime/kernel/ecs/ECS_registry.h"
#include "../../../../../source/runtime/dna/DNA_object_type.h"

namespace qt::scene {

void SCN_init_default_scene()
{
  // Create test entities only once
  vektor::kernel::create_entity(nullptr,
                                nullptr,
                                "Cylinder",
                                vektor::dna::DNA_ENTITY_CYLINDER,
                                vektor::dna::DNA_MESH,
                                0.0f,
                                0.0f,
                                0.0f,
                                1.0f,
                                0.5f,
                                0.2f);

  vektor::kernel::create_entity(nullptr,
                                nullptr,
                                "Cylinder_2",
                                vektor::dna::DNA_ENTITY_CYLINDER,
                                vektor::dna::DNA_MESH,
                                10.0f,
                                0.0f,
                                10.0f,
                                0.6f,
                                0.8f,
                                0.4f);
}

} // namespace qt::scene
