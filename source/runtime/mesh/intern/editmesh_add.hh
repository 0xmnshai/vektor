#pragma once

#include "../mesh.h"
#include "../../dna/DNA_object_type.h"
#include "../../kernel/ecs/ECS_mesh_primitives.h"

namespace vektor::mesh {

void mesh_add_plane(struct dna::Object *obj, float size)
{ 
  kernel::add_primitive_plane_exec(obj, size);
}

void mesh_add_cube(struct dna::Object *obj, float size)
{
  kernel::add_primitive_cube_exec(obj, size);
}

void mesh_add_cylinder(struct dna::Object *obj, float radius, float depth, int segments)
{
  kernel::add_primitive_cylinder_exec(obj, radius, depth, segments);
}

}  // namespace vektor::mesh
