#pragma once

#include "RNA_types.h"
#include <vector>

namespace vektor::rna {
struct RNAProperty {
  const char *identifier;
  const char *name;
  const char *description;

  int offset;

  PropertyType type;
  PropertySubType sub_type;
};

struct RNAStruct {
  const char *identifier;
  const char *name;
  const char *description;

  std::vector<RNAProperty *> properties;
  RNAStruct *base;  // For inheritance if needed
};

struct VektorRNA {
  std::vector<RNAStruct *> rna_structs;
};
}  // namespace vektor::rna
