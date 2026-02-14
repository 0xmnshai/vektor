

#pragma once

#include "DNA_defs.h"
#include "DNA_id.h"
#include "DNA_id_enum.h"

#include "DNA_listBase.h"

struct ScrAreaMap
{
};

namespace vektor
{

namespace bke
{
struct WindowManagerRuntime;
struct WindowRuntime;
} // namespace bke

namespace ui
{
struct Layout;
}

struct wmNotifier;
struct wmWindow;
struct wmWindowManager;

struct wmEvent_ConsecutiveData;
struct wmEvent;
struct wmKeyConfig;
struct wmKeyMap;
struct wmMsgBus;
struct wmOperator;
struct wmOperatorType;

struct PointerRNA;
struct Report;
struct ReportList;
struct Stereo3dFormat;
struct vkContext;
struct bScreen;
struct wmTimer;

#define OP_MAX_TYPENAME 64
#define KMAP_MAX_NAME 64

#
#
struct ReportTimerInfo
{
    float widthfac       = 0;
    float flash_progress = 0;
};

enum
{
    WM_EXTENSIONS_UPDATE_UNSET    = -2,
    WM_EXTENSIONS_UPDATE_CHECKING = -1,
};

enum
{
    WM_INIT_FLAG_WINDOW    = (1 << 0),
    WM_INIT_FLAG_KEYCONFIG = (1 << 1),
};

enum
{
    WM_OUTLINER_SYNC_SELECT_FROM_OBJECT    = (1 << 0),
    WM_OUTLINER_SYNC_SELECT_FROM_EDIT_BONE = (1 << 1),
    WM_OUTLINER_SYNC_SELECT_FROM_POSE_BONE = (1 << 2),
    WM_OUTLINER_SYNC_SELECT_FROM_SEQUENCE  = (1 << 3),
};

#define WM_OUTLINER_SYNC_SELECT_FROM_ALL                                                                               \
    (WM_OUTLINER_SYNC_SELECT_FROM_OBJECT | WM_OUTLINER_SYNC_SELECT_FROM_EDIT_BONE |                                    \
     WM_OUTLINER_SYNC_SELECT_FROM_POSE_BONE | WM_OUTLINER_SYNC_SELECT_FROM_SEQUENCE)

struct wmWindowManager
{
#ifdef __cplusplus

    static constexpr blender::ID_Type id_type = blender::ID_WM;
#endif

    blender::ID                id;

    ListBaseT<wmWindow>        windows;

    uint8_t                    init_flag                  = 0;
    char                       _pad0[1]                   = {};

    short                      file_saved                 = 0;

    short                      op_undo_depth              = 0;

    short                      outliner_sync_select_dirty = 0;

    int                        extensions_updates         = 0;

    int                        extensions_blocked         = 0;

    struct wmTimer*            autosavetimer              = nullptr;

    char                       autosave_scheduled         = 0;
    char                       _pad2[7]                   = {};

    bke::WindowManagerRuntime* runtime                    = nullptr;
};

#define WM_KEYCONFIG_ARRAY_P(wm) &(wm)->runtime->defaultconf, &(wm)->runtime->addonconf, &(wm)->runtime->userconf

#define WM_KEYCONFIG_STR_DEFAULT "Blender"

#if !(defined(WIN32) || defined(__APPLE__)) && !defined(DNA_DEPRECATED)
#ifdef __GNUC__
#define ime_data ime_data __attribute__((deprecated))
#endif
#endif

struct wmWindow
{

    struct wmWindow *               next = nullptr, *prev = nullptr;

    struct wmWindow*                parent              = nullptr;

    struct Scene*                   scene               = nullptr;

    struct Scene*                   new_scene           = nullptr;

    char                            view_layer_name[64] = "";

    struct Scene*                   unpinned_scene      = nullptr;

    struct WorkSpaceInstanceHook*   workspace_hook      = nullptr;

    ScrAreaMap                      global_areas;

    DNA_DEPRECATED struct bScreen*  screen = nullptr;

    int                             winid  = 0;

    short                           posx = 0, posy = 0;

    short                           sizex = 0, sizey = 0;

    char                            windowstate                           = 0;

    char                            active                                = 0;

    short                           cursor                                = 0;

    short                           lastcursor                            = 0;

    short                           modalcursor                           = 0;

    short                           grabcursor                            = 0;

    short                           pie_event_type_lock                   = 0;

    short                           pie_event_type_last                   = 0;

    char                            tag_cursor_refresh                    = 0;

    char                            event_queue_check_click               = 0;

    char                            event_queue_check_drag                = 0;

    char                            event_queue_check_drag_handled        = 0;

    short                           event_queue_consecutive_gesture_type  = 0;

    int                             event_queue_consecutive_gesture_xy[2] = {};

    struct wmEvent_ConsecutiveData* event_queue_consecutive_gesture_data  = nullptr;

    char                            addmousemove                          = 0;
    char                            _pad1[7]                              = {};

    struct Stereo3dFormat*          stereo3d_format                       = nullptr;

    bke::WindowRuntime*             runtime                               = nullptr;
};

#ifdef ime_data
#undef ime_data
#endif

#
#
struct wmOperatorTypeMacro
{
    struct wmOperatorTypeMacro *next = nullptr, *prev = nullptr;

    char                        idname[64] = "";

    struct IDProperty*          properties = nullptr;
    struct PointerRNA*          ptr        = nullptr;
};

struct wmKeyMapItem
{
    struct wmKeyMapItem *next = nullptr, *prev = nullptr;

    char                 idname[64]        = "";

    IDProperty*          properties        = nullptr;

    char                 propvalue_str[64] = "";

    short                propvalue         = 0;

    short                type              = 0;

    int8_t               val               = 0;

    int8_t               direction         = 0;

    int8_t               shift             = 0;
    int8_t               ctrl              = 0;
    int8_t               alt               = 0;

    int8_t               oskey             = 0;

    int8_t               hyper             = 0;

    char                 _pad0[7]          = {};

    short                keymodifier       = 0;

    uint8_t              flag              = 0;

    uint8_t              maptype           = 0;

    short                id                = 0;

    struct PointerRNA*   ptr               = nullptr;
};

struct wmKeyMapDiffItem
{
    struct wmKeyMapDiffItem *next = nullptr, *prev = nullptr;

    wmKeyMapItem*            remove_item = nullptr;
    wmKeyMapItem*            add_item    = nullptr;
};

enum
{
    KMI_INACTIVE      = (1 << 0),
    KMI_EXPANDED      = (1 << 1),
    KMI_USER_MODIFIED = (1 << 2),
    KMI_UPDATE        = (1 << 3),

    KMI_REPEAT_IGNORE = (1 << 4),
};

enum
{
    KMI_TYPE_KEYBOARD  = 0,
    KMI_TYPE_MOUSE     = 1,

    KMI_TYPE_TEXTINPUT = 3,
    KMI_TYPE_TIMER     = 4,
    KMI_TYPE_NDOF      = 5,
};

enum
{

    KEYMAP_MODAL             = (1 << 0),

    KEYMAP_USER              = (1 << 1),
    KEYMAP_EXPANDED          = (1 << 2),
    KEYMAP_CHILDREN_EXPANDED = (1 << 3),

    KEYMAP_DIFF              = (1 << 4),

    KEYMAP_USER_MODIFIED     = (1 << 5),
    KEYMAP_UPDATE            = (1 << 6),

    KEYMAP_TOOL              = (1 << 7),
};

struct wmKeyMap
{
    struct wmKeyMap *           next = nullptr, *prev = nullptr;

    ListBaseT<wmKeyMapItem>     items;
    ListBaseT<wmKeyMapDiffItem> diff_items;

    char                        idname[64]    = "";

    short                       spaceid       = 0;

    short                       regionid      = 0;

    char                        owner_id[128] = "";

    short                       flag          = 0;

    short                       kmi_id        = 0;

    bool (*poll)(struct vkContext*);
    bool (*poll_modal_item)(const struct wmOperator* op,
                            int                      value) = {};

    const void* modal_items            = nullptr;
};

struct wmKeyConfigPref
{
    struct wmKeyConfigPref *next = nullptr, *prev = nullptr;

    char                    idname[64] = "";
    IDProperty*             prop       = nullptr;
};

enum
{
    KEYCONF_USER         = (1 << 1),
    KEYCONF_INIT_DEFAULT = (1 << 2),
};

struct wmKeyConfig
{
    struct wmKeyConfig *next = nullptr, *prev = nullptr;

    char                idname[64]   = "";

    char                basename[64] = "";

    ListBaseT<wmKeyMap> keymaps;
    int                 actkeymap = 0;
    short               flag      = 0;
    char                _pad0[2]  = {};
};

struct wmOperator
{
    struct wmOperator *    next = nullptr, *prev = nullptr;

    char                   idname[64]  = "";

    IDProperty*            properties  = nullptr;

    struct wmOperatorType* type        = nullptr;

    void*                  customdata  = nullptr;

    void*                  py_instance = nullptr;

    struct PointerRNA*     ptr         = nullptr;

    struct ReportList*     reports     = nullptr;

    ListBaseT<wmOperator>  macro;

    struct wmOperator*     opm     = nullptr;

    ui::Layout*            layout  = nullptr;
    short                  flag    = 0;
    char                   _pad[6] = {};
};

} // namespace vektor
