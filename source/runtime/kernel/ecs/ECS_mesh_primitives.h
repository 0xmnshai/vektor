#pragma once

#include <glm/glm.hpp>

namespace vektor::dna {
struct Object;
}

namespace vektor::kernel {

// VMO Primitives (New System)
void add_primitive_cube_exec(dna::Object *obj, float size);
void add_primitive_cylinder_exec(dna::Object *obj, float radius, float depth, int segments);
void add_primitive_plane_exec(dna::Object *obj, float size);

}  // namespace vektor::kernel
