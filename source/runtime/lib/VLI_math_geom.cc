#include <algorithm>
#include <cfloat>
#include <glm/glm.hpp>

#include "VLI_math_geom.h"

namespace vektor::lib {

bool ray_triangle_intersect(const glm::vec3 &ray_origin,
                            const glm::vec3 &ray_dir,
                            const glm::vec3 &v0,
                            const glm::vec3 &v1,
                            const glm::vec3 &v2,
                            float &t)
{
  const float EPSILON = 1e-8f;
  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;
  glm::vec3 h = glm::cross(ray_dir, edge2);
  float a = glm::dot(edge1, h);

  if (a > -EPSILON && a < EPSILON) {
    return false;  // Ray is parallel to triangle
  }

  float f = 1.0f / a;
  glm::vec3 s = ray_origin - v0;
  float u = f * glm::dot(s, h);

  if (u < 0.0f || u > 1.0f) {
    return false;
  }

  glm::vec3 q = glm::cross(s, edge1);
  float v = f * glm::dot(ray_dir, q);

  if (v < 0.0f || u + v > 1.0f) {
    return false;
  }

  // At this stage we can compute t to find out where the intersection point is on the line.
  float t_temp = f * glm::dot(edge2, q);

  if (t_temp > EPSILON) {
    t = t_temp;
    return true;
  }

  return false;
}

bool ray_aabb_intersect(const glm::vec3 &ray_origin,
                        const glm::vec3 &ray_dir,
                        const glm::vec3 &min,
                        const glm::vec3 &max,
                        float &t_near)
{
  glm::vec3 inv_dir = 1.0f / ray_dir;
  glm::vec3 t0 = (min - ray_origin) * inv_dir;
  glm::vec3 t1 = (max - ray_origin) * inv_dir;

  glm::vec3 t_min = glm::min(t0, t1);
  glm::vec3 t_max = glm::max(t0, t1);

  float t_near_val = std::max({t_min.x, t_min.y, t_min.z});
  float t_far_val = std::min({t_max.x, t_max.y, t_max.z});

  if (t_near_val <= t_far_val && t_far_val > 0) {
    t_near = t_near_val;
    return true;
  }

  return false;
}

float ray_mesh_intersect(const glm::vec3 &ray_origin,
                         const glm::vec3 &ray_dir,
                         const dna::Mesh *mesh)
{
  if (!mesh || !mesh->mvert || !mesh->mloop || !mesh->mpoly) {
    return FLT_MAX;
  }

  float closest_t = FLT_MAX;

  for (int i = 0; i < mesh->faces_num; ++i) {
    const dna::MPoly &poly = mesh->mpoly[i];
    int num_corners = poly.num_corners;
    int first_corner = poly.first_corner;

    if (num_corners < 3) {
      continue;
    }

    // Use fan triangulation for convex n-gons
    const glm::vec3 &v0 = mesh->mvert[mesh->mloop[first_corner].v].co;
    for (int j = 1; j < num_corners - 1; ++j) {
      const glm::vec3 &v1 = mesh->mvert[mesh->mloop[first_corner + j].v].co;
      const glm::vec3 &v2 = mesh->mvert[mesh->mloop[first_corner + j + 1].v].co;

      float t;
      if (ray_triangle_intersect(ray_origin, ray_dir, v0, v1, v2, t)) {
        if (t < closest_t) {
          closest_t = t;
        }
      }
    }
  }

  return closest_t;
}

}  // namespace vektor::lib


/**
TODO: 

BVH acceleration   (massive improvement)
2. SoA mesh layout    (large improvement)
3. SIMD triangle test (large improvement)
4. multithreading     (large improvement)

we can implement such things in rust for improvements 

*/