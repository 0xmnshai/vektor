#pragma once

#include "RNA_internal.h"
#include "RNA_types.h"

namespace vektor::rna {
#ifdef __cplusplus
extern "C" {
#endif

float RNA_property_float_get_index(PointerRNA *ptr, RNAProperty *prop, int index);
void RNA_property_float_set_index(PointerRNA *ptr, RNAProperty *prop, int index, float value);

void RNA_property_float_get_array(PointerRNA *ptr, RNAProperty *prop, float *values);
void RNA_property_float_set_array(PointerRNA *ptr, RNAProperty *prop, const float *values);

#ifdef __cplusplus
}
#endif
}  // namespace vektor::rna
