#pragma once

#include "DNA_id.h"
#include "DNA_material_types.h"

namespace vektor::dna {

typedef struct MVert {
  glm::vec3 co;
  glm::vec3 no;
  glm::vec2 uv;
} MVert;

typedef struct MPoly {
  int first_corner;
  int num_corners;
} MPoly;

typedef struct MEdge {
  int v1;
  int v2;
} MEdge;

typedef struct MLoop {
  int v;
  int e;
  int f;
  glm::vec2 uv;
} MLoop;

typedef struct Mesh {
  ID id;

  /** The number of vertices in the mesh, and the size of #vert_data. */
  int verts_num = 0;
  /** The number of edges in the mesh, and the size of #edge_data. */
  int edges_num = 0;
  /** The number of faces in the mesh, and the size of #face_data. */
  int faces_num = 0;
  /** The number of face corners in the mesh, and the size of #corner_data. */
  int corners_num = 0;

  MPoly *mpoly = nullptr;  // pointer to  mesh polygons
  MLoop *mloop = nullptr;  // pointer to face corners
  MVert *mvert = nullptr;  // pointer to vertices

  Material material;

} Mesh;
}  // namespace vektor::dna

/**

  // we will move these method in rna so that we can use it 
   std::vector<glm::vec3> mesh_vert_positions(Mesh* mesh) ;
   std::vector<glm::vec3> mesh_vert_normals(Mesh* mesh) ;
   std::vector<glm::vec2> mesh_vert_uvs(Mesh* mesh) ;

   std::vector<glm::vec3> mesh_vert_positions_to_write(Mesh* mesh) ;
   std::vector<glm::vec3> mesh_vert_normals_to_write(Mesh* mesh) ;
   std::vector<glm::vec2> mesh_vert_uvs_to_write(Mesh ) ;

*/
