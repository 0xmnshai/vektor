#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"

#include "../../dna/DNA_object_type.h"
#include "../../kernel/ecs/ECS_registry.h"
#include "../RNA_ecs_registry.h"

namespace vektor::rna {

using namespace vektor::kernel;

extern "C" {

void *RNA_ecs_get_registry(void)
{
  return (void *)&ECSRegistry::instance();
}

unsigned int RNA_ecs_create_entity(void)
{
  return (unsigned int)ECSRegistry::instance().create_entity();
}

dna::Object *RNA_ecs_get_object(ECSRegistry *registry, entt::entity entity)
{
  return &registry->add_component<dna::Object>(entity);
}

bool RNA_ecs_is_selected(ECSRegistry *registry, entt::entity entity)
{
  if (registry->registry().any_of<dna::Selected>(entity)) {
    return registry->registry().get<dna::Selected>(entity).selected;
  }
  return false;
}

void RNA_ecs_set_selected(ECSRegistry *registry, entt::entity entity, bool selected)
{
  registry->emplace_or_replace<dna::Selected>(entity, selected);
}

void RNA_ecs_set_active(ECSRegistry *registry, entt::entity entity, bool active)
{
  if (active) {
    auto view = registry->registry().view<dna::Active>();
    for (auto ent : view) {
      registry->emplace_or_replace<dna::Active>(ent, false);
    }
  }
  registry->emplace_or_replace<dna::Active>(entity, active);
}

void RNA_ecs_destroy_entity(ECSRegistry *registry, entt::entity entity)
{
  registry->destroy_entity(entity);
}
}
}  // namespace vektor::rna
