#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../dna/DNA_object_type.h"

namespace vektor::rna {
inline glm::mat4 RNA_object_to_mat4(dna::Object *ob)
{
  glm::mat4 mat = glm::mat4(1.0f);
  mat = glm::translate(mat, ob->transform.location);
  mat = glm::rotate(mat, ob->transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  mat = glm::rotate(mat, ob->transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  mat = glm::rotate(mat, ob->transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
  mat = glm::scale(mat, ob->transform.scale);
  return mat;
}
}  // namespace vektor::rna
