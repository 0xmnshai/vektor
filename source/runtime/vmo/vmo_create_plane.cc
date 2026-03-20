#include "VMO_execute.h"

namespace vektor::vmo {

void vmo_create_plane_exec(dna::Mesh *mesh, float size)
{
  float off = size / 2.0f;

  mesh->verts_num = 4;
  mesh->faces_num = 2;
  mesh->corners_num = 8;

  mesh->mvert = new dna::MVert[4];
  mesh->mpoly = new dna::MPoly[2];
  mesh->mloop = new dna::MLoop[8];

  // Vertices
  mesh->mvert[0].co = glm::vec3(-off, 0.0f, -off);
  mesh->mvert[1].co = glm::vec3(-off, 0.0f, off);
  mesh->mvert[2].co = glm::vec3(off, 0.0f, off);
  mesh->mvert[3].co = glm::vec3(off, 0.0f, -off);

  // Restore vertical normals to fix lighting
  mesh->mvert[0].no = glm::vec3(0.0f, 1.0f, 0.0f);
  mesh->mvert[1].no = glm::vec3(0.0f, 1.0f, 0.0f);
  mesh->mvert[2].no = glm::vec3(0.0f, 1.0f, 0.0f);
  mesh->mvert[3].no = glm::vec3(0.0f, 1.0f, 0.0f);

  // Face 1 (Top)
  mesh->mpoly[0].first_corner = 0;
  mesh->mpoly[0].num_corners = 4;

  // Face 2 (Bottom)
  mesh->mpoly[1].first_corner = 4;
  mesh->mpoly[1].num_corners = 4;

  // Loops for Face 1
  for (int i = 0; i < 4; i++) {
    mesh->mloop[i].v = i;
  }

  // Loops for Face 2 (Reversed for bottom face)
  mesh->mloop[4].v = 3;
  mesh->mloop[5].v = 2;
  mesh->mloop[6].v = 1;
  mesh->mloop[7].v = 0;
}

}  // namespace vektor::vmo
