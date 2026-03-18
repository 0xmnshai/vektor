#include "VMO_execute.h"
#include <cmath>

namespace vektor::vmo {

void vmo_create_cylinder_exec(dna::Mesh *mesh, float radius, float depth, int segments)
{
  float halfDepth = depth / 2.0f;
  int num_verts = segments * 2 + 2;  // +2 for centers of caps
  int num_faces = segments * 3;      // Side quads + top triangles + bottom triangles
  int num_loops = segments * 4 + segments * 3 + segments * 3;

  mesh->verts_num = num_verts;
  mesh->faces_num = num_faces;
  mesh->corners_num = num_loops;

  mesh->mvert = new dna::MVert[num_verts];
  mesh->mpoly = new dna::MPoly[num_faces];
  mesh->mloop = new dna::MLoop[num_loops];

  // Vertices creation
  for (int i = 0; i < segments; i++) {
    float angle = (float)i / (float)segments * 2.0f * (float)M_PI;
    float x = std::cos(angle) * radius;
    float z = std::sin(angle) * radius;

    mesh->mvert[i].co = glm::vec3(x, halfDepth, z);               // Top
    mesh->mvert[i + segments].co = glm::vec3(x, -halfDepth, z);   // Bottom
    
    // Calculate normals for smooth shading on the cylinder sides
    glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
    mesh->mvert[i].no = normal;
    mesh->mvert[i + segments].no = normal;
  }
  
  // Centers for caps
  int v_top_center = segments * 2;
  int v_bottom_center = segments * 2 + 1;
  mesh->mvert[v_top_center].co = glm::vec3(0, halfDepth, 0);
  mesh->mvert[v_top_center].no = glm::vec3(0, 1.0f, 0);
  mesh->mvert[v_bottom_center].co = glm::vec3(0, -halfDepth, 0);
  mesh->mvert[v_bottom_center].no = glm::vec3(0, -1.0f, 0);

  int f_idx = 0;
  int l_idx = 0;

  for (int i = 0; i < segments; i++) {
    int next_i = (i + 1) % segments;

    // Side Quad
    mesh->mpoly[f_idx].first_corner = l_idx;
    mesh->mpoly[f_idx].num_corners = 4;
    mesh->mloop[l_idx++].v = i;
    mesh->mloop[l_idx++].v = i + segments;
    mesh->mloop[l_idx++].v = next_i + segments;
    mesh->mloop[l_idx++].v = next_i;
    f_idx++;

    // Top Triangle
    mesh->mpoly[f_idx].first_corner = l_idx;
    mesh->mpoly[f_idx].num_corners = 3;
    mesh->mloop[l_idx++].v = v_top_center;
    mesh->mloop[l_idx++].v = next_i;
    mesh->mloop[l_idx++].v = i;
    f_idx++;

    // Bottom Triangle
    mesh->mpoly[f_idx].first_corner = l_idx;
    mesh->mpoly[f_idx].num_corners = 3;
    mesh->mloop[l_idx++].v = v_bottom_center;
    mesh->mloop[l_idx++].v = i + segments;
    mesh->mloop[l_idx++].v = next_i + segments;
    f_idx++;
  }
}

}  // namespace vektor::vmo
