#include "Entity.h"

namespace vektor::kernel {

Entity::Entity(entt::entity handle, ECSRegistry *registry)
    : entity_handle_(handle), registry_(registry)
{
}

}  // namespace vektor::kernel
