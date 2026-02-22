#include "VKE_lib_id.hh"

#include "../../intern/gaurdalloc/MEM_gaurdalloc.hh"

namespace vektor
{

void BLO_readfile_id_runtime_data_free(ID& id)
{
    MEM_SAFE_DELETE(id.runtime->readfile_data);
}

void VKE_libblock_free_runtime_data(ID* id)
{
    if (id->runtime)
    {
        /* During "normal" file loading this data is released when versioning ends. Some versioning
         * code also deletes IDs, though. For example, in the startup blend file, brushes that were
         * replaced by assets are deleted. This means that the regular "delete this ID" flow (aka this
         * code here) also needs to free this data. */
        BLO_readfile_id_runtime_data_free(*id);

        MEM_SAFE_DELETE(id->runtime);
    }
}

} // namespace vektor