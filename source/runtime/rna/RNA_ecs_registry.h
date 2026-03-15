#pragma once

#include <entt/entt.hpp>

#include "../dna/DNA_object_type.h"
#include "../kernel/ecs/ECS_registry.h"

namespace vektor::rna {

#ifdef __cplusplus
extern "C" {
#endif

void *RNA_ecs_get_registry(void);
unsigned int RNA_ecs_create_entity(void);
dna::Object *RNA_ecs_get_object(kernel::ECSRegistry *registry, entt::entity entity);
bool RNA_ecs_is_selected(kernel::ECSRegistry *registry, entt::entity entity);
void RNA_ecs_set_selected(kernel::ECSRegistry *registry, entt::entity entity, bool selected);
void RNA_ecs_set_active(kernel::ECSRegistry *registry, entt::entity entity, bool active);
#ifdef __cplusplus
}
#endif

}  // namespace vektor::rna
