#include "VKE_callbacks.hh"
#include "../rna/RNA_types.h"

namespace vektor
{
void VKE_callback_exec_string(Main*       vkmain,
                              const char* str,
                              eCbEvent    evt)
{
    PrimitiveStringRNA data = {nullptr};
    data.value              = str;
    PointerRNA  str_ptr     = RNA_pointer_create_discrete(nullptr, RNA_PrimitiveString, &data);

    PointerRNA* pointers[1] = {&str_ptr};

    VKE_callback_exec(vkmain, pointers, ARRAY_SIZE(pointers), evt);
}
} // namespace vektor