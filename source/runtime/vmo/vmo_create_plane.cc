#include "VMO_execute.h"

namespace vektor::vmo {

void vmo_create_plane_exec(dna::Mesh *mesh, float size)
{
  float off = size / 2.0f;

  mesh->verts_num = 4;
  mesh->faces_num = 1;
  mesh->corners_num = 4;

  mesh->mvert = new dna::MVert[4];
  mesh->mpoly = new dna::MPoly[1];
  mesh->mloop = new dna::MLoop[4];

  // Vertices
  mesh->mvert[0].co = glm::vec3(-off, 0.0f, -off);
  mesh->mvert[1].co = glm::vec3(-off, 0.0f, off);
  mesh->mvert[2].co = glm::vec3(off, 0.0f, off);
  mesh->mvert[3].co = glm::vec3(off, 0.0f, -off);

  for (int i = 0; i < 4; i++) {
    mesh->mvert[i].no = glm::vec3(0.0f, 1.0f, 0.0f);
  }

  // Face
  mesh->mpoly[0].first_corner = 0;
  mesh->mpoly[0].num_corners = 4;

  // Loops
  for (int i = 0; i < 4; i++) {
    mesh->mloop[i].v = i;
  }
}

}  // namespace vektor::vmo
