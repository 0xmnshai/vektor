#include "VMO_execute.h"
#include <cmath>
#include <glm/glm.hpp>

namespace vektor::vmo {

#ifndef M_PI
#  define M_PI 3.14159265358979323846f
#endif

void vmo_create_light_exec(dna::Mesh *mesh, float size)
{
  int segments = 12;
  int rings = 8;
  mesh->verts_num = (rings + 1) * (segments + 1);
  mesh->faces_num = rings * segments;
  mesh->corners_num = mesh->faces_num * 4;

  mesh->mvert = new dna::MVert[mesh->verts_num];
  mesh->mpoly = new dna::MPoly[mesh->faces_num];
  mesh->mloop = new dna::MLoop[mesh->corners_num];

  int v_idx = 0;
  for (int r = 0; r <= rings; r++) {
    float phi = (float)M_PI * (float)r / (float)rings;
    for (int s = 0; s <= segments; s++) {
      float theta = 2.0f * (float)M_PI * (float)s / (float)segments;

      float x = size * sinf(phi) * cosf(theta);
      float y = size * cosf(phi);
      float z = size * sinf(phi) * sinf(theta);

      mesh->mvert[v_idx].co = glm::vec3(x, y, z);
      mesh->mvert[v_idx].no = glm::normalize(glm::vec3(x, y, z));
      mesh->mvert[v_idx].uv = glm::vec2((float)s / (float)segments, (float)r / (float)rings);
      v_idx++;
    }
  }

  int p_idx = 0;
  int l_idx = 0;
  for (int r = 0; r < rings; r++) {
    for (int s = 0; s < segments; s++) {
      int v1 = r * (segments + 1) + s;
      int v2 = v1 + 1;
      int v3 = (r + 1) * (segments + 1) + s + 1;
      int v4 = v3 - 1;

      mesh->mpoly[p_idx].first_corner = l_idx;
      mesh->mpoly[p_idx].num_corners = 4;

      mesh->mloop[l_idx++].v = v1;
      mesh->mloop[l_idx++].v = v2;
      mesh->mloop[l_idx++].v = v3;
      mesh->mloop[l_idx++].v = v4;

      p_idx++;
    }
  }
}

}  // namespace vektor::vmo
