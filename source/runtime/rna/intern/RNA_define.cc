#include "../RNA_define.h"
#include "../dna/DNA_object_type.h"

namespace vektor::rna {

const char *rna_enum_object_type_to_string(dna::ObjectType type)
{
  switch (type) {
    case dna::ObjectType::Empty:
      return "empty";
    case dna::ObjectType::Mesh:
      return "mesh";
    case dna::ObjectType::Camera:
      return "camera";
    case dna::ObjectType::Light:
      return "light";
    default:
      return "unknown";
  }
}
}  // namespace vektor::rna
