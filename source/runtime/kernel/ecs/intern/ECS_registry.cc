#include "entt/entt.hpp"
#include <glm/glm.hpp>
#include <string>

#include "../../dna/DNA_object_type.h"
#include "../../rna/RNA_ecs_registry.h"
#include "../../rna/RNA_internal.h"
#include "../../rna/RNA_types.h"
#include "../ECS_registry.h"
#include "ECS_mesh_primitives.h"

extern "C" {
void outliner_notify_scene_changed();
}

namespace vektor::kernel {
void create_entity(rna::VektorRNA *v_rna,
                   rna::RNAStruct *rna_,
                   const char *name,
                   const char *description,
                   int type,
                   float x,
                   float y,
                   float z,
                   float r,
                   float g,
                   float b)
{
  auto &registry = ECSRegistry::instance();
  auto entity = (entt::entity)rna::RNA_ecs_create_entity();

  auto object = (dna::Object *)rna::RNA_ecs_get_object(&registry, entity);

  object->type = (dna::ObjectType)type;
  strncpy(object->id.name, name, sizeof(object->id.name) - 1);
  object->id.name[sizeof(object->id.name) - 1] = '\0';

  strncpy(object->description, description, sizeof(object->description) - 1);
  object->description[sizeof(object->description) - 1] = '\0';

  object->type = (dna::ObjectType)type;

  object->transform.location = glm::vec3(x, y, z);
  object->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  object->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

  if (object->type == dna::ObjectType::Mesh) {
    if (std::string(name).find("Cube") != std::string::npos) {
      add_primitive_cube_exec(object, 1.0f);
    }
    else if (std::string(name).find("Plane") != std::string::npos) {
      add_primitive_plane_exec(object, 10.0f);
    }
    else {
      add_primitive_cylinder_exec(object, 1.0f, 2.0f, 32);
    }
  }
  else if (object->type == dna::ObjectType::Light) {
    add_primitive_light_exec(object, 1.0f);

    std::string n = name;
    object->light->type = dna::LA_LOCAL;

    // Position lights a bit higher by default
    object->transform.location.y = 5.0f;
  }

  // Apply material color from creation parameters
  if (object->mesh && !object->mesh->materials.empty()) {
    object->mesh->materials[0]->color.r = r;
    object->mesh->materials[0]->color.g = g;
    object->mesh->materials[0]->color.b = b;
  }

  // Shader creation requires an active GPU context (OpenGL/Metal), which is
  // only guaranteed during rendering (DRW_draw_view). Shaders are created
  // lazily on the first draw call for each object.
  object->shader_program = nullptr;

  // rna::PointerRNA object_ptr = rna::RNA_pointer_from_entity<dna::Object>(
  //     registry.registry(), entity, object_type);

  outliner_notify_scene_changed();
}

void destroy_entity(entt::entity entity)
{
  ECSRegistry::instance().destroy_entity(entity);
  outliner_notify_scene_changed();
}
}  // namespace vektor::kernel
