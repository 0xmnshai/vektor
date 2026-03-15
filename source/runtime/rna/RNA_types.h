#pragma once

namespace vektor::rna {
struct RNAStruct;

typedef enum PropertyType {
  PROP_BOOLEAN = 0,
  PROP_INT = 1,
  PROP_FLOAT = 2,
  PROP_STRING = 3,
  PROP_ENUM = 4,
  PROP_POINTER = 5,
  PROP_COLLECTION = 6,
} PropertyType;

typedef enum PropertySubType {
  PROP_NONE = 0,
  PROP_XYZ = 1,
  PROP_COLOR = 2,
  PROP_DISTANCE = 3,
  PROP_ANGLE = 4,
} PropertySubType;

struct PropertyRNA {
  char name[64];
  PropertyType type;
  PropertySubType sub_type;
  void *data;
};

struct PointerRNA {
  RNAStruct *type;
  void *data;
};

}  // namespace vektor::rna
