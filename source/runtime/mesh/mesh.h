#pragma once

#include "../dna/DNA_object_type.h"
 
// we will declare everything regarding mesh here then will define in inter/mesh.cc
namespace vektor::mesh {

void mesh_add_plane(struct dna::Object *obj, float size);
void mesh_add_cube(struct dna::Object *obj, float size);
void mesh_add_cylinder(struct dna::Object *obj, float radius, float depth, int segments);

}  // namespace vektor::meshx
