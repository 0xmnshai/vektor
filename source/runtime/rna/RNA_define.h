#pragma once

#include "DNA_object_type.h"
#include "RNA_internal.h"
#include "RNA_types.h"

namespace vektor::rna {

extern "C" {

// TODO: these functions are still need to be implemented
RNAStruct *RNA_def_struct(VektorRNA *rna, const char *name);
RNAProperty *RNA_def_property(RNAStruct *srna, const char *name, int type, int subtype);

void RNA_def_property_float_sdna(RNAProperty *prop, const char *ignore, const char *name);
void RNA_def_property_ui_text(RNAProperty *prop, const char *name, const char *description);

const char *rna_enum_object_type_to_string(dna::ObjectType type);
}
}  // namespace vektor::rna
