#pragma once

#include "RNA_internal_types.h"
#include "rna_aceess.h"

namespace vektor
{

#define RNA_AnyType nullptr

bool RNA_struct_is_a(const StructRNA* type,
                     const StructRNA* srna)
{
    const StructRNA* base;

    if (srna == RNA_AnyType)
    {
        return true;
    }

    if (!type)
    {
        return false;
    }

    /* ptr->type is always maximally refined */
    for (base = type; base; base = base->base)
    {
        if (base == srna)
        {
            return true;
        }
    }

    return false;
}

} // namespace vektor
