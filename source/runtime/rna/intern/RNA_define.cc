#include "../RNA_define.h"
#include "../dna/DNA_object_type.h"

namespace vektor::rna {

const char *rna_enum_object_type_to_string(dna::ObjectTypeDNA type)
{
  switch (type) {
    case dna::DNA_COLLECTION:
      return "collection";
    case dna::DNA_MESH:
      return "mesh";
    case dna::DNA_CAMERA:
      return "camera";
    case dna::DNA_LIGHT:
      return "light";
    default:
      return "unknown";
  }
}
}  // namespace vektor::rna
