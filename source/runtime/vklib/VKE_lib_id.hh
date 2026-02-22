#pragma once

#include "../dna/DNA_id.h"
#include "VKE_main.hh"

namespace vektor
{
void VKE_libblock_free_data_py(ID* id);
void VKE_libblock_free_runtime_data(ID* id);

enum
{
    LIB_ID_COPY_ACTIONS                           = 1 << 24,
    /** EXCEPTION! Deep-copy shape-keys used by copied obdata ID. */
    LIB_ID_COPY_SHAPEKEY                          = 1 << 26,
    /**
     * EXCEPTION! Deep-copy screen used by copied workspace ID.
     * WARNING: Should always be used, except in `NO_MAIN` cases of copying. */
    LIB_ID_COPY_SCREEN                            = 1 << 27,
    /** EXCEPTION! Specific deep-copy of node trees used e.g. for rendering purposes. */
    LIB_ID_COPY_NODETREE_LOCALIZE                 = 1 << 28,
    /**
     * EXCEPTION! Specific handling of RB objects regarding collections differs depending whether we
     * duplicate scene/collections, or objects.
     */
    LIB_ID_COPY_RIGID_BODY_NO_COLLECTION_HANDLING = 1 << 29,
    /* Copy asset metadata. */
    LIB_ID_COPY_ASSET_METADATA                    = 1 << 30,

    /* *** Helper 'defines' gathering most common flag sets. *** */
    /**
     * Shape-keys are not real ID's, more like local data to geometry IDs. Same for bScreens being
     * local data of Workspaces.
     */
    LIB_ID_COPY_DEFAULT                           = LIB_ID_COPY_SHAPEKEY | LIB_ID_COPY_SCREEN,

    LIB_ID_CREATE_NO_ALLOCATE                     = 1 << 2,

};

struct ID* VKE_id_copy_in_lib(Main*                    bmain,
                              std::optional<Library*>  owner_library,
                              const ID*                id,
                              std::optional<const ID*> new_owner_id,
                              ID**                     new_id_p,
                              int                      flag);

ID*        VKE_id_copy_ex(Main*     bmain,
                          const ID* id,
                          ID**      new_id_p,
                          int       flag);

void       VKE_libblock_copy_in_lib(Main*                    bmain,
                                    std::optional<Library*>  owner_library,
                                    const ID*                id,
                                    std::optional<const ID*> new_owner_id,
                                    ID**                     new_id_p,
                                    int                      orig_flag);

void       id_lib_extern(ID* id);

void       id_us_plus(ID* id);

} // namespace vektor