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
#include "../../creator_global.h"
#include <string>

extern "C" {
void outliner_notify_scene_changed();
}

namespace vektor::kernel {
void create_entity(rna::VektorRNA *v_rna,
                   rna::RNAStruct *object_type,
                   const char *name,
                   int type,
                   int object_type_dna,
                   float x = 0.0f,
                   float y = 0.0f,
                   float z = 0.0f,
                   float r = 1.0f,
                   float g = 1.0f,
                   float b = 1.0f)
{
  auto &registry = ECSRegistry::instance();
  auto entity = (entt::entity)rna::RNA_ecs_create_entity();

  auto object = (dna::Object *)rna::RNA_ecs_get_object(&registry, entity);

  object->type = (dna::EntityTypeDNA)type;
  strncpy(object->id_name, name, sizeof(object->id_name) - 1);
  object->id_name[sizeof(object->id_name) - 1] = '\0';

  object->object_type = (dna::ObjectTypeDNA)object_type_dna;

  object->transform.location = glm::vec3(x, y, z);
  object->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  object->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

  object->material.color = glm::vec4(r, g, b, 1.0f);

  if (object->type == dna::DNA_ENTITY_CYLINDER) {
    std::string base_path = vektor::lib::get_application_dir_path();
    std::string vert_path =
        base_path + "/../../source/runtime/gpu/shaders/file/mesh/cylinder/cylinder.vert";
    std::string frag_path =
        base_path + "/../../source/runtime/gpu/shaders/file/mesh/cylinder/cylinder.frag";

    vektor::gpu::GPUShaderSourceParameters params = {nullptr};
    if (creator::G.gpu_backend == creator::GPU_BACKEND_METAL) {
      vert_path = base_path + "/../../source/runtime/gpu/shaders/file/mesh/cylinder/cylinder.metal";
      frag_path = vert_path;
      params.vert_entry = "vertex_main";
      params.frag_entry = "fragment_main";
    }

    object->shader_program = gpu::GPU_shader_create_from_source(vert_path.c_str(),
                                                                frag_path.c_str(),
                                                                &params);
  }
  else {
    object->shader_program = nullptr;
  }

  rna::PointerRNA object_ptr = rna::RNA_pointer_from_entity<dna::Object>(
      registry.registry(), entity, object_type);

  outliner_notify_scene_changed();
}
}  // namespace vektor::kernel
