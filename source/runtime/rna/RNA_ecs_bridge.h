#pragma once

#include <entt/entt.hpp>

#include "RNA_types.h"

namespace vektor::rna {
template<typename T>
vektor::rna::PointerRNA RNA_pointer_from_entity(entt::registry &reg,
                                                entt::entity entity,
                                                vektor::rna::RNAStruct *type)
{
  vektor::rna::PointerRNA ptr = {};
  ptr.type = type;
  if (reg.all_of<T>(entity)) {
    ptr.data = &reg.get<T>(entity);
  }
  else {
    ptr.data = nullptr;
  }
  return ptr;
}
}  // namespace vektor::rna
