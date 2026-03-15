#pragma once

#include <entt/entt.hpp>

#include "ECS_registry.h"

namespace vektor::kernel {

class Entity {
 public:
  Entity() = default;
  Entity(entt::entity handle, ECSRegistry *registry);
  Entity(const Entity &other) = default;

  template<typename T, typename... Args> T &add_component(Args &&...args)
  {
    return registry_->add_component<T>(entity_handle_, std::forward<Args>(args)...);
  }

  template<typename T> T &get_component()
  {
    return registry_->get_component<T>(entity_handle_);
  }

  template<typename T> bool has_component()
  {
    return registry_->has_component<T>(entity_handle_);
  }

  template<typename T> void remove_component()
  {
    registry_->remove_component<T>(entity_handle_);
  }

  explicit operator bool() const
  {
    return entity_handle_ != entt::null;
  }
  explicit operator entt::entity() const
  {
    return entity_handle_;
  }
  explicit operator uint32_t() const
  {
    return (uint32_t)entity_handle_;
  }

  bool operator==(const Entity &other) const
  {
    return entity_handle_ == other.entity_handle_ && registry_ == other.registry_;
  }

  bool operator!=(const Entity &other) const
  {
    return !(*this == other);
  }

 private:
  entt::entity entity_handle_{entt::null};
  ECSRegistry *registry_{nullptr};
};

}  // namespace vektor::kernel
