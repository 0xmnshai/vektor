#include "VMO_execute.h"

namespace vektor::vmo {

void vmo_create_cube_exec(dna::Mesh *mesh, float size)
{
  float off = size / 2.0f;

  mesh->verts_num = 24;
  mesh->faces_num = 6;
  mesh->corners_num = 24;

  mesh->mvert = new dna::MVert[mesh->verts_num];
  mesh->mpoly = new dna::MPoly[mesh->faces_num];
  mesh->mloop = new dna::MLoop[mesh->corners_num];

  // Faces and Vertices configuration
  // Front, Back, Top, Bottom, Right, Left
  glm::vec3 normals[6] = {
      glm::vec3( 0.0f,  0.0f,  1.0f), // Front
      glm::vec3( 0.0f,  0.0f, -1.0f), // Back
      glm::vec3( 0.0f,  1.0f,  0.0f), // Top
      glm::vec3( 0.0f, -1.0f,  0.0f), // Bottom
      glm::vec3( 1.0f,  0.0f,  0.0f), // Right
      glm::vec3(-1.0f,  0.0f,  0.0f)  // Left
  };

  glm::vec3 positions[6][4] = {
      // Front (z = off)
      { {-off, -off,  off}, { off, -off,  off}, { off,  off,  off}, {-off,  off,  off} },
      // Back (z = -off)
      { { off, -off, -off}, {-off, -off, -off}, {-off,  off, -off}, { off,  off, -off} },
      // Top (y = off)
      { {-off,  off,  off}, { off,  off,  off}, { off,  off, -off}, {-off,  off, -off} },
      // Bottom (y = -off)
      { {-off, -off, -off}, { off, -off, -off}, { off, -off,  off}, {-off, -off,  off} },
      // Right (x = off)
      { { off, -off,  off}, { off, -off, -off}, { off,  off, -off}, { off,  off,  off} },
      // Left (x = -off)
      { {-off, -off, -off}, {-off, -off,  off}, {-off,  off,  off}, {-off,  off, -off} }
  };

  int v_idx = 0;
  for (int i = 0; i < 6; i++) {
    mesh->mpoly[i].first_corner = v_idx;
    mesh->mpoly[i].num_corners = 4;
    
    for (int j = 0; j < 4; j++) {
      mesh->mvert[v_idx].co = positions[i][j];
      mesh->mvert[v_idx].no = normals[i];
      // Basic UV layout
      mesh->mvert[v_idx].uv = glm::vec2((j == 1 || j == 2) ? 1.0f : 0.0f, (j == 2 || j == 3) ? 1.0f : 0.0f);
      
      mesh->mloop[v_idx].v = v_idx;
      v_idx++;
    }
  }
}

}  // namespace vektor::vmo
