#include "entt/entt.hpp"
#include <glm/glm.hpp>

#include "../../dna/DNA_object_type.h"
#include "../../rna/RNA_ecs_registry.h"
#include "../../rna/RNA_internal.h"
#include "../../rna/RNA_types.h"
#include "../ECS_registry.h"

#include "../../creator_global.h"
#include "../../gpu/GPU_shader.h"
#include "../../lib/intern/appdir.h"
#include <string>

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

    if (object->mesh && !object->mesh->materials.empty()) {
      object->mesh->materials[0]->color.r = r;
      object->mesh->materials[0]->color.g = g;
      object->mesh->materials[0]->color.b = b;
    }

    vektor::gpu::GPUShaderSourceParameters params = {nullptr};

    const std::string dir_path = std::string(lib::get_application_dir_path()) + "/../../source/runtime/gpu";

    const std::string vert_path = dir_path + "/shaders/core/program/program.vert";
    const std::string frag_path = dir_path + "/shaders/core/program/program.frag";
    const std::string metal_path = dir_path + "/shaders/core/program/program.metal";

    if (creator::G.gpu_backend == creator::GPU_BACKEND_METAL) {
      params.vert_entry = "vertex_main";
      params.frag_entry = "fragment_main";
      // const char *metal_src = "";
      object->shader_program = gpu::GPU_shader_create_from_source(
          metal_path.c_str(), metal_path.c_str(), &params);
    }
    else {
      // const char *vert_src = "";
      // const char *frag_src = "";
      object->shader_program = gpu::GPU_shader_create_from_source(
          vert_path.c_str(), frag_path.c_str(), &params);
    }
  }
  else {
    object->shader_program = nullptr;
  }

  // rna::PointerRNA object_ptr = rna::RNA_pointer_from_entity<dna::Object>(
  //     registry.registry(), entity, object_type);

  outliner_notify_scene_changed();
}
}  // namespace vektor::kernel
