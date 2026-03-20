#include "ECS_mesh_primitives.h"

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include "../../dna/DNA_object_type.h"
#include "../../vmo/VMO_execute.h"

namespace vektor::kernel {

void add_primitive_cube_exec(dna::Object *obj, float size)
{
  obj->mesh = std::make_shared<dna::Mesh>();
  vmo::vmo_create_cube_exec(obj->mesh.get(), size);
  
  auto mat = std::make_shared<dna::Material>();
  // we have decrease the alpha value for now, to check 
  mat->color = dna::Color(0.26f, 0.27f, 0.29f, 1.0f);
  strcpy(mat->name, "DefaultMaterial");
  obj->mesh->materials.push_back(mat);
}

void add_primitive_cylinder_exec(dna::Object *obj, float radius, float depth, int segments)
{
  obj->mesh = std::make_shared<dna::Mesh>();
  vmo::vmo_create_cylinder_exec(obj->mesh.get(), radius, depth, segments);
  
  auto mat = std::make_shared<dna::Material>();
  mat->color = dna::Color(0.26f, 0.27f, 0.29f, 1.0f);
  strcpy(mat->name, "DefaultMaterial");
  obj->mesh->materials.push_back(mat);
}

void add_primitive_plane_exec(dna::Object *obj, float size)
{
  obj->mesh = std::make_shared<dna::Mesh>();
  vmo::vmo_create_plane_exec(obj->mesh.get(), size);
  
  auto mat = std::make_shared<dna::Material>();
  mat->color = dna::Color(0.26f, 0.27f, 0.29f, 1.0f);
  strcpy(mat->name, "DefaultMaterial");
  obj->mesh->materials.push_back(mat);
}

void add_primitive_light_exec(dna::Object *obj, float size)
{
  /* Initialize Light DNA */
  obj->light = std::make_shared<dna::DNA_Light>();
  obj->light->type = dna::LA_LOCAL;
  obj->light->color = glm::vec3(1.0f, 1.0f, 1.0f);
  obj->light->energy = 1.0f;
  obj->light->distance = 25.0f; // Default range

  /* Create Visual Bulb Mesh */
  obj->mesh = std::make_shared<dna::Mesh>();
  vmo::vmo_create_light_exec(obj->mesh.get(), size * 0.5f);

  auto mat = std::make_shared<dna::Material>();
  mat->color = dna::Color(obj->light->color.r, obj->light->color.g, obj->light->color.b, 1.0f); // Primary icon color
  strcpy(mat->name, "LightBulbMaterial");
  mat->emissive_color = dna::Color(obj->light->color.r, obj->light->color.g, obj->light->color.b, 1.0f); // Glow color
  obj->mesh->materials.push_back(mat);
}

}  // namespace vektor::kernel
