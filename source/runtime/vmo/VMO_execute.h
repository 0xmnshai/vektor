#pragma once

#include "../dna/DNA_mesh_types.h"

namespace vektor::vmo {

void vmo_create_cube_exec(dna::Mesh *mesh, float size);
void vmo_create_plane_exec(dna::Mesh *mesh, float size);
void vmo_create_cylinder_exec(dna::Mesh *mesh, float radius, float depth, int segments);

void vmo_create_light_exec(dna::Mesh *mesh, float size);

}  // namespace vektor::vmo
