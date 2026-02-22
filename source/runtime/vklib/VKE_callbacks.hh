#pragma once

#include "VKE_main.hh"

#include "../rna/RNA_types.h"

namespace vektor
{

enum eCbEvent
{
    VKE_CB_EVT_LOAD_PRE,
    VKE_CB_EVT_LOAD_POST,
    VKE_CB_EVT_LOAD_POST_FAIL,
    VKE_CB_EVT_SAVE_PRE,
    VKE_CB_EVT_SAVE_POST,
    VKE_CB_EVT_SAVE_POST_FAIL,
    VKE_CB_EVT_UNDO_PRE,
    VKE_CB_EVT_UNDO_POST,
    VKE_CB_EVT_REDO_PRE,
    VKE_CB_EVT_REDO_POST,
    VKE_CB_EVT_DEPSGRAPH_UPDATE_PRE,
    VKE_CB_EVT_DEPSGRAPH_UPDATE_POST,
    VKE_CB_EVT_VERSION_UPDATE,
    VKE_CB_EVT_LOAD_FACTORY_USERDEF_POST,
    VKE_CB_EVT_LOAD_FACTORY_STARTUP_POST,
    VKE_CB_EVT_XR_SESSION_START_PRE,
    VKE_CB_EVT_ANNOTATION_PRE,
    VKE_CB_EVT_ANNOTATION_POST,
    VKE_CB_EVT_OBJECT_BAKE_PRE,
    VKE_CB_EVT_OBJECT_BAKE_COMPLETE,
    VKE_CB_EVT_OBJECT_BAKE_CANCEL,
    VKE_CB_EVT_COMPOSITE_PRE,
    VKE_CB_EVT_COMPOSITE_POST,
    VKE_CB_EVT_COMPOSITE_CANCEL,
    VKE_CB_EVT_ANIMATION_PLAYBACK_PRE,
    VKE_CB_EVT_ANIMATION_PLAYBACK_POST,
    VKE_CB_EVT_TRANSLATION_UPDATE_POST,
    VKE_CB_EVT_EXTENSION_REPOS_UPDATE_PRE,
    VKE_CB_EVT_EXTENSION_REPOS_UPDATE_POST,
    VKE_CB_EVT_EXTENSION_REPOS_SYNC,
    VKE_CB_EVT_EXTENSION_REPOS_FILES_CLEAR,
    VKE_CB_EVT_VKENDIMPORT_PRE,
    VKE_CB_EVT_VKENDIMPORT_POST,
    VKE_CB_EVT_EXIT_PRE,
    VKE_CB_EVT_TOT,
};

void VKE_callback_exec_string(Main*       vkmain,
                              const char* str,
                              eCbEvent    evt);

void VKE_callback_exec(Main*        vkmain,
                       PointerRNA** pointers,
                       int          pointers_num,
                       eCbEvent     evt);
}; // namespace vektor