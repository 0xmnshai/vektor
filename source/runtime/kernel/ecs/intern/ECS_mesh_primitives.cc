#include "ECS_mesh_primitives.h"

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include "../../vmo/VMO_execute.h"
#include "../../dna/DNA_object_type.h"

namespace vektor::kernel {

void create_cylinder_mesh(std::vector<Vertex> &vertices, float radius, float height, int segments)
{
  vertices.clear();
  float halfHeight = height / 2.0f;

  for (int i = 0; i < segments; ++i) {
    float angle1 = (float)i / (float)segments * 2.0f * (float)M_PI;
    float angle2 = (float)(i + 1) / (float)segments * 2.0f * (float)M_PI;

    float x1 = std::cos(angle1) * radius;
    float z1 = std::sin(angle1) * radius;
    float x2 = std::cos(angle2) * radius;
    float z2 = std::sin(angle2) * radius;

    // Side triangles (two triangles per segment)
    vertices.push_back({{x1, -halfHeight, z1}, {std::cos(angle1), 0.0f, std::sin(angle1)}});
    vertices.push_back({{x1, halfHeight, z1}, {std::cos(angle1), 0.0f, std::sin(angle1)}});
    vertices.push_back({{x2, -halfHeight, z2}, {std::cos(angle2), 0.0f, std::sin(angle2)}});

    vertices.push_back({{x2, -halfHeight, z2}, {std::cos(angle2), 0.0f, std::sin(angle2)}});
    vertices.push_back({{x1, halfHeight, z1}, {std::cos(angle1), 0.0f, std::sin(angle1)}});
    vertices.push_back({{x2, halfHeight, z2}, {std::cos(angle2), 0.0f, std::sin(angle2)}});

    // Top cap
    vertices.push_back({{0.0f, halfHeight, 0.0f}, {0.0f, 1.0f, 0.0f}});
    vertices.push_back({{x1, halfHeight, z1}, {0.0f, 1.0f, 0.0f}});
    vertices.push_back({{x2, halfHeight, z2}, {0.0f, 1.0f, 0.0f}});

    // Bottom cap
    vertices.push_back({{0.0f, -halfHeight, 0.0f}, {0.0f, -1.0f, 0.0f}});
    vertices.push_back({{x2, -halfHeight, z2}, {0.0f, -1.0f, 0.0f}});
    vertices.push_back({{x1, -halfHeight, z1}, {0.0f, -1.0f, 0.0f}});
  }
}

void add_primitive_cube_exec(dna::Object *obj, float size)
{
  obj->mesh = std::make_shared<dna::Mesh>();
  vmo::vmo_create_cube_exec(obj->mesh.get(), size);
}

void add_primitive_cylinder_exec(dna::Object *obj, float radius, float depth, int segments)
{
  obj->mesh = std::make_shared<dna::Mesh>();
  vmo::vmo_create_cylinder_exec(obj->mesh.get(), radius, depth, segments);
}

void add_primitive_plane_exec(dna::Object *obj, float size)
{
  obj->mesh = std::make_shared<dna::Mesh>();
  vmo::vmo_create_plane_exec(obj->mesh.get(), size);
}

}  // namespace vektor::kernel
