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
#ifdef __cplusplus
}
#endif

}  // namespace vektor::rna
