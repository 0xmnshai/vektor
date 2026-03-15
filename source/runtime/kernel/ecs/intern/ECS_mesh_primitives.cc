#include "ECS_mesh_primitives.h"
#include <cmath>

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

namespace vektor::kernel {

void create_cylinder_mesh(std::vector<Vertex> &vertices, float radius, float height, int segments)
{
  vertices.clear();
  float halfHeight = height / 2.0f;

  for (int i = 0; i < segments; ++i) {
    float angle1 = (float)i / (float)segments * 2.0f * 3.1415927f;
    float angle2 = (float)(i + 1) / (float)segments * 2.0f * 3.1415927f;

    float x1 = std::cos(angle1) * radius;
    float z1 = std::sin(angle1) * radius;
    float x2 = std::cos(angle2) * radius;
    float z2 = std::sin(angle2) * radius;

    // Side triangles (two triangles per segment)
    // Triangle 1
    vertices.push_back({{x1, -halfHeight, z1}, {std::cos(angle1), 0.0f, std::sin(angle1)}});
    vertices.push_back({{x2, -halfHeight, z2}, {std::cos(angle2), 0.0f, std::sin(angle2)}});
    vertices.push_back({{x1, halfHeight, z1}, {std::cos(angle1), 0.0f, std::sin(angle1)}});

    // Triangle 2
    vertices.push_back({{x2, -halfHeight, z2}, {std::cos(angle2), 0.0f, std::sin(angle2)}});
    vertices.push_back({{x2, halfHeight, z2}, {std::cos(angle2), 0.0f, std::sin(angle2)}});
    vertices.push_back({{x1, halfHeight, z1}, {std::cos(angle1), 0.0f, std::sin(angle1)}});

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

}  // namespace vektor::kernel
