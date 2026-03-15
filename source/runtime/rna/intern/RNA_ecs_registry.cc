#include "entt/entt.hpp"

#include "../../dna/DNA_object_type.h"
#include "../../kernel/ecs/ECS_registry.h"
#include "../RNA_ecs_registry.h"

namespace vektor::rna {

using namespace vektor::kernel;

extern "C" {

void *RNA_ecs_get_registry(void)
{
  return (void *)&ECSRegistry::instance().registry();
}

unsigned int RNA_ecs_create_entity(void)
{
  return (unsigned int)ECSRegistry::instance().create_entity();
}

dna::Object *RNA_ecs_get_object(ECSRegistry *registry, entt::entity entity)
{
  return &registry->add_component<dna::Object>(entity);
}
}
}  // namespace vektor::rna
