#pragma once

#include <glm/glm.hpp>

#include "../../dna/DNA_vertex_.h"

namespace vektor::dna {
struct Object;
}

namespace vektor::kernel {

// TODO: we will remove this function and will try using the exec one, and remove dependecies of this function as well 
// VPI primitives (Legacy) 
void create_cylinder_mesh(std::vector<dna::Vertex> &vertices, float radius, float height, int segments);

// VMO Primitives (New System)
void add_primitive_cube_exec(dna::Object *obj, float size);
void add_primitive_cylinder_exec(dna::Object *obj, float radius, float depth, int segments);
void add_primitive_plane_exec(dna::Object *obj, float size);

}  // namespace vektor::kernel
