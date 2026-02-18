#pragma once

#include "../dna/DNA_listBase.h" // IWYU pragma: export
#include "VKE_compiler_attrs.h"

namespace vektor
{
void VKE_addtail(ListBase* listbase,
                 void*     vlink) ATTR_NONNULL(1);
}