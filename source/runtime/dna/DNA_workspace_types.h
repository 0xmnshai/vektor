#pragma once

#include "DNA_id.h"
#include "DNA_listBase.h"
namespace vektor
{

struct WorkSpaceDataRelation
{
    struct WorkSpaceDataRelation *next = nullptr, *prev = nullptr;

    /** The data used to identify the relation
     * (e.g. to find screen-layout (= value) from/for a hook).
     * NOTE: Now runtime only. */
    void*                         parent    = nullptr;
    /** The value for this parent-data/workspace relation. */
    void*                         value     = nullptr;

    /** Reference to the actual parent window, #wmWindow.winid. Used in read/write code. */
    int                           parentid  = 0;
    char                          _pad_0[4] = {};
};

struct WorkSpaceLayout
{
    struct WorkSpaceLayout *next = nullptr, *prev = nullptr;

    struct bScreen*         screen                = nullptr;
    /* The name of this layout, we override the RNA name of the screen with this
     * (but not ID name itself) */
    char                    name[/*MAX_NAME*/ 64] = "";
};

struct WorkSpace
{
#ifdef __cplusplus
    /** See #ID_Type comment for why this is here. */
    static constexpr ID_Type id_type = ID_WS;
#endif

    ID                                      id;

    // ListBaseT<WorkSpaceLayout>              layouts               = {nullptr, nullptr};
    /* Store for each hook (so for each window) which layout has
     * been activated the last time this workspace was visible. */
    ListBaseT<struct WorkSpaceDataRelation> hook_layout_relations = {nullptr, nullptr};

    ListBaseT<WorkSpaceLayout>              layouts               = {nullptr, nullptr};

    /* Feature tagging (use for addons) */
    // ListBaseT<wmOwnerID>                    owner_ids             = {nullptr, nullptr};

    /** Optional, scene to switch to when enabling this workspace (NULL to disable). Cleared on
     * link/append. */
    struct Scene*                           pin_scene             = nullptr;

    /* Scene that is used by the sequence editors in this workspace. */
    struct Scene*                           sequencer_scene       = nullptr;

    char                                    _pad[4]               = {};

    int                                     object_mode           = 0;

    /** Enum eWorkSpaceFlags. */
    int                                     flags                 = 0;

    /** Number for workspace tab reordering in the UI. */
    int                                     order                 = 0;

    /** Info text from modal operators (runtime). */
    // WorkSpaceRuntime*                  runtime               = nullptr;

    /** Workspace-wide active asset library, for asset UIs to use (e.g. asset view UI template). The
     * Asset Browser has its own and doesn't use this. */
    // AssetLibraryReference                   asset_library_ref;

    /**
     * Ground truth for the currently active viewer node. When a viewer node is activated its path is
     * set here. Editors can check here for which node is active (currently the node editor,
     * spreadsheet and viewport do this).
     */
    // ViewerPath                              viewer_path;
};

struct WorkSpaceInstanceHook
{
    WorkSpace*              active               = nullptr;
    struct WorkSpaceLayout* act_layout           = nullptr;

    /**
     * Needed because we can't change work-spaces/layouts in running handler loop,
     * it would break context.
     */
    WorkSpace*              temp_workspace_store = nullptr;
    struct WorkSpaceLayout* temp_layout_store    = nullptr;
};
} // namespace vektor