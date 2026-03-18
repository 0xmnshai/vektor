#ifndef BKE_OBJECT_COMPONENT_H
#define BKE_OBJECT_COMPONENT_H

#include <QString>

namespace vektor::kernel {
struct Object;
};

namespace vektor::kernel {
// we can use this to extend this on objects, like Phyiscs Component, etc..
class VKE_ObjectComponent {
 public:
  virtual ~VKE_ObjectComponent() = default;

  virtual void on_register() {}
  virtual void on_unregister() {}
  virtual void on_transform_updated() {}

  [[nodiscard]] Object *owner() const
  {
    return m_owner;
  }
  void set_owner(Object *ob)
  {
    m_owner = ob;
  }

 private:
  Object *m_owner = nullptr;
};
};  // namespace vektor::kernel

#endif  // BKE_OBJECT_COMPONENT_H

/**

Flow :
Object* cube = new Object();

// on creating a new cube  ? 
we will pass this to cube pritive which will create cube using bmo_create_cube_exec .

auto* render = new RenderComponent();
render->set_owner(cube);

cube->components.push_back(render);
render->on_register();

cube->loc += glm::vec3(1, 0, 0);

for (auto* c : cube->components) {
    c->on_transform_updated();
}


this can by made dynamic

maybe we can create a render system ?
*/
