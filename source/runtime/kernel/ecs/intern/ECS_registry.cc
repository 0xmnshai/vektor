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

#include "ECS_mesh_primitives.h"

extern "C" {
void outliner_notify_scene_changed();
}

namespace vektor::kernel {
void create_entity(rna::VektorRNA *v_rna,
                   rna::RNAStruct *object_type,
                   const char *name,
                   int type,
                   int object_type_dna,
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

  strncpy(object->description,
          "Constructed mesh object",
          sizeof(object->description) - 1);
  object->description[sizeof(object->description) - 1] = '\0';

  object->type = (dna::ObjectType)type;

  object->transform.location = glm::vec3(x, y, z);
  object->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  object->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

  object->mesh = std::make_shared<dna::Mesh>();
  object->mesh->material.color.r = r;
  object->mesh->material.color.g = g;
  object->mesh->material.color.b = b;
  object->mesh->material.color.a = 1.0f;

  if (object->type == dna::ObjectType::Mesh) {
    if (std::string(name).find("Cube") != std::string::npos) {
      add_primitive_cube_exec(object, 1.0f);
    } else if (std::string(name).find("Plane") != std::string::npos) {
      add_primitive_plane_exec(object, 10.0f);
    } else {
      add_primitive_cylinder_exec(object, 1.0f, 2.0f, 32); 
    }

    std::string base_path = vektor::lib::get_application_dir_path();
    std::string vert_path = base_path +
                            "/../../source/runtime/gpu/shaders/core/cylinder/cylinder.vert";
    std::string frag_path = base_path +
                            "/../../source/runtime/gpu/shaders/core/cylinder/cylinder.frag";

    vektor::gpu::GPUShaderSourceParameters params = {nullptr};
    if (creator::G.gpu_backend == creator::GPU_BACKEND_METAL) {
      vert_path = base_path + "/../../source/runtime/gpu/shaders/core/cylinder/cylinder.metal";
      frag_path = vert_path;
      params.vert_entry = "vertex_main";
      params.frag_entry = "fragment_main";
    }

    object->shader_program = gpu::GPU_shader_create_from_source(
        vert_path.c_str(), frag_path.c_str(), &params);
  }
  else {
    object->shader_program = nullptr;
  }

  rna::PointerRNA object_ptr = rna::RNA_pointer_from_entity<dna::Object>(
      registry.registry(), entity, object_type);

  outliner_notify_scene_changed();
}
}  // namespace vektor::kernel
