#pragma once

#include "../loader/VLO_readfile.h"
#include "wm_api.h"

namespace vektor
{
extern void wm_clear_default_size(vkContext* C);

extern void wm_add_default(Main*      bmain,
                           vkContext* C);
extern void wm_file_read_setup_wm_finalize(vkContext*                C,
                                           Main*                     bmain,
                                           BlendFileReadWMSetupData* wm_setup_data);

extern void wm_close_and_free(vkContext* C,
                              wmWindowManager*);

extern void WM_check(vkContext* C);

} // namespace vektor