#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vektor::kernel {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

void create_cylinder_mesh(std::vector<Vertex>& vertices, float radius, float height, int segments);

} // namespace vektor::kernel
