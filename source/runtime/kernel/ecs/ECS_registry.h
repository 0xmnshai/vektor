#pragma once

#include <entt/entt.hpp>

#include "../../rna/RNA_internal.h"

namespace vektor::kernel {
class ECSRegistry {
 public:
  static ECSRegistry &instance()
  {
    static ECSRegistry s;
    return s;
  }

  entt::registry &registry()
  {
    return registry_;
  }

  entt::entity create_entity()
  {
    return registry_.create();
  }

  template<typename T, typename... Args> T &add_component(entt::entity entity, Args &&...args)
  {
    return registry_.emplace<T>(entity, std::forward<Args>(args)...);
  }

  template<typename T, typename... Args> T &emplace_or_replace(entt::entity entity, Args &&...args)
  {
    return registry_.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
  }

  template<typename T> T &get_component(entt::entity entity)
  {
    return registry_.get<T>(entity);
  }

  template<typename T> void remove_component(entt::entity entity)
  {
    registry_.remove<T>(entity);
  }

  template<typename T> bool has_component(entt::entity entity)
  {
    return registry_.all_of<T>(entity);
  }

  template<typename T> void clear_component(entt::entity entity)
  {
    registry_.remove<T>(entity);
  }

  explicit operator entt::registry &()
  {
    return registry_;
  }

 private:
  ECSRegistry() = default;
  entt::registry registry_;
};

void create_entity(rna::VektorRNA *v_rna,
                   rna::RNAStruct *object_type,
                   const char *name,
                   int type,
                   int object_type_dna,
                   float r,
                   float g,
                   float b);
}  // namespace vektor::kernel
