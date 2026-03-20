#pragma once

#include <glm/glm.hpp>
#include "../dna/DNA_mesh_types.h"

namespace vektor::lib {

/**
 * Perform a ray-triangle intersection using the Möller-Trumbore algorithm.
 * Returns true if the ray intersects the triangle, and sets 't' to the distance.
 */
bool ray_triangle_intersect(const glm::vec3 &ray_origin,
                            const glm::vec3 &ray_dir,
                            const glm::vec3 &v0,
                            const glm::vec3 &v1,
                            const glm::vec3 &v2,
                            float &t);

/**
 * Perform a ray-AABB intersection.
 * Returns true if the ray hits the box, and sets 't_near' to the distance.
 */
bool ray_aabb_intersect(const glm::vec3 &ray_origin,
                        const glm::vec3 &ray_dir,
                        const glm::vec3 &min,
                        const glm::vec3 &max,
                        float &t_near);

/**
 * Perform a ray-mesh intersection.
 * Iterates through all triangles of the mesh and returns the closest intersection distance.
 * Returns FLT_MAX if no intersection occurs.
 */
float ray_mesh_intersect(const glm::vec3 &ray_origin,
                         const glm::vec3 &ray_dir,
                         const dna::Mesh *mesh);

}  // namespace vektor::lib
