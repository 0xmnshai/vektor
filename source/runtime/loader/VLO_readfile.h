#pragma once

#include "../dna/DNA_windowmanager_types.h"
#include "../vklib/VKE_main.hh"

namespace vektor
{
struct BlendFileReadWMSetupData
{
    /** The existing WM when file-reading process is started. */
    wmWindowManager* old_wm;

    /** The startup file is being read. */
    bool             is_read_homefile;
    /** The factory startup file is being read. */
    bool             is_factory_startup;
};

void BLO_readfile_id_runtime_data_free_all(Main& bmain);

void BLO_readfile_id_runtime_data_free(ID& id);


} // namespace vektor