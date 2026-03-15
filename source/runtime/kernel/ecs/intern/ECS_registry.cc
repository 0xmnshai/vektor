#include "entt/entt.hpp"
#include <glm/glm.hpp>

#include "../../dna/DNA_object_type.h"
#include "../../rna/RNA_ecs_bridge.h"
#include "../../rna/RNA_ecs_registry.h"
#include "../../rna/RNA_internal.h"
#include "../../rna/RNA_types.h"
#include "../ECS_registry.h"

#include "../../gpu/GPU_shader.h"
#include "../../lib/intern/appdir.h"
#include <string>

namespace vektor::kernel {
void create_entity(rna::VektorRNA *v_rna,
                   rna::RNAStruct *object_type,
                   const char *name,
                   int type,
                   int object_type_dna,
                   float r,
                   float g,
                   float b)
{
  auto *registry = (ECSRegistry *)rna::RNA_ecs_get_registry();
  auto entity = (entt::entity)rna::RNA_ecs_create_entity();

  auto object = (dna::Object *)rna::RNA_ecs_get_object(registry, entity);

  object->type = (dna::EntityTypeDNA)type;
  strncpy(object->id_name, name, sizeof(object->id_name) - 1);
  object->id_name[sizeof(object->id_name) - 1] = '\0';

  object->object_type = (dna::ObjectTypeDNA)object_type_dna;

  object->transform.location = glm::vec3(0.0f, 0.0f, 0.0f);
  object->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  object->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

  object->material.color = glm::vec4(r, g, b, 1.0f);

  // Automatic shader attachment for cylinders
  if (object->type == dna::DNA_ENTITY_CYLINDER) {
    std::string base_path = vektor::lib::get_application_dir_path();
    std::string vert_path = base_path +
                            "/../../source/runtime/gpu/shaders/file/mesh/cylinder/cylinder.vert";
    std::string frag_path = base_path +
                            "/../../source/runtime/gpu/shaders/file/mesh/cylinder/cylinder.frag";
    object->shader_program = gpu::GPU_shader_create_from_source(vert_path.c_str(),
                                                                frag_path.c_str());
  }
  else {
    object->shader_program = nullptr;
  }

  rna::PointerRNA object_ptr = rna::RNA_pointer_from_entity<dna::Object>(
      registry->registry(), entity, object_type);
}
}  // namespace vektor::kernel
