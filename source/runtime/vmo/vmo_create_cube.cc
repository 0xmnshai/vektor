#include "VMO_execute.h"

namespace vektor::vmo {

void vmo_create_cube_exec(dna::Mesh *mesh, float size)
{
  float off = size / 2.0f;

  mesh->verts_num = 8;
  mesh->faces_num = 6;
  mesh->corners_num = 24;

  mesh->mvert = new dna::MVert[mesh->verts_num];
  mesh->mpoly = new dna::MPoly[mesh->faces_num];
  mesh->mloop = new dna::MLoop[mesh->corners_num];

  // Vertices
  int v_idx = 0;
  for (int x : {-1, 1}) {
    for (int y : {-1, 1}) {
      for (int z : {-1, 1}) {
        mesh->mvert[v_idx].co = glm::vec3((float)x * off, (float)y * off, (float)z * off);
        mesh->mvert[v_idx].no = glm::normalize(mesh->mvert[v_idx].co);
        v_idx++;
      }
    }
  }

  // Faces and Loops
  const int faces[6][4] = {
      {0, 1, 3, 2}, {2, 3, 7, 6}, {6, 7, 5, 4}, {4, 5, 1, 0}, {2, 6, 4, 0}, {7, 3, 1, 5}};

  int l_idx = 0;
  for (int i = 0; i < 6; i++) {
    mesh->mpoly[i].first_corner = l_idx;
    mesh->mpoly[i].num_corners = 4;
    for (int j = 0; j < 4; j++) {
      mesh->mloop[l_idx].v = faces[i][j];
      l_idx++;
    }
  }
}

}  // namespace vektor::vmo
