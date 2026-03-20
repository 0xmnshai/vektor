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

}  // namespace vektor::kernel
