#pragma once

#include "../dna/DNA_defs.h"
#include "../windowmanager/wm_files.h"
#include "VKE_main.hh"

using namespace vektor::vklib;

namespace vektor
{

enum eBLOReadSkip
{
    BLO_READ_SKIP_NONE          = 0,
    /** Skip #BLO_CODE_USER blocks. */
    BLO_READ_SKIP_USERDEF       = (1 << 0),
    /** Only read #BLO_CODE_USER (and associated data). */
    BLO_READ_SKIP_DATA          = (1 << 1),
    /** Do not attempt to re-use IDs from old bmain for unchanged ones in case of undo. */
    BLO_READ_SKIP_UNDO_OLD_MAIN = (1 << 2),
};
ENUM_OPERATORS(eBLOReadSkip)
#define BLO_READ_SKIP_ALL (BLO_READ_SKIP_USERDEF | BLO_READ_SKIP_DATA)

// implmenet this in user def types .h
enum eUserPref_Flag
{
    USER_AUTOSAVE                     = (1 << 0),
    USER_FLAG_NUMINPUT_ADVANCED       = (1 << 1),
    USER_FLAG_RECENT_SEARCHES_DISAVKE = (1 << 2),
    USER_MENU_CLOSE_LEAVE             = (1 << 3),
    USER_FLAG_UNUSED_4                = (1 << 4), /* cleared */
    USER_TRACKBALL                    = (1 << 5),
    USER_FLAG_UNUSED_6                = (1 << 6), /* cleared */
    USER_FLAG_UNUSED_7                = (1 << 7), /* cleared */
    USER_MAT_ON_OB                    = (1 << 8),
    USER_INTERNET_ALLOW               = (1 << 9),
    USER_DEVELOPER_UI                 = (1 << 10),
    USER_TOOLTIPS                     = (1 << 11),
    USER_TWOBUTTONMOUSE               = (1 << 12),
    USER_NONUMPAD                     = (1 << 13),
    USER_ADD_CURSORALIGNED            = (1 << 14),
    USER_FILECOMPRESS                 = (1 << 15),
    USER_FLAG_UNUSED_5                = (1 << 16), /* dirty */
    USER_CUSTOM_RANGE                 = (1 << 17),
    USER_ADD_EDITMODE                 = (1 << 18),
    USER_ADD_VIEWALIGNED              = (1 << 19),
    USER_RELPATHS                     = (1 << 20),
    USER_RELEASECONFIRM               = (1 << 21),
    USER_SCRIPT_AUTOEXEC_DISAVKE      = (1 << 22),
    USER_FILENOUI                     = (1 << 23),
    USER_NONEGFRAMES                  = (1 << 24),
    USER_TXT_TABSTOSPACES_DISAVKE     = (1 << 25),
    USER_TOOLTIPS_PYTHON              = (1 << 26),
    USER_FLAG_UNUSED_27               = (1 << 27), /* dirty */
    USER_HIDE_DOT_DATABLOCK           = (1 << 28),
};

enum eUserPref_PrefFlag
{
    USER_PREF_FLAG_SAVE = (1 << 0),
};

enum eTimecodeStyles
{
    /**
     * As little info as is necessary to show relevant info with '+' to denote the frames
     * i.e. HH:MM:SS+FF, MM:SS+FF, SS+FF, or MM:SS.
     */
    USER_TIMECODE_MINIMAL      = 0,
    /** Reduced SMPTE - (HH:)MM:SS:FF */
    USER_TIMECODE_SMPTE_MSF    = 1,
    /** Full SMPTE - HH:MM:SS:FF */
    USER_TIMECODE_SMPTE_FULL   = 2,
    /** Milliseconds for sub-frames - HH:MM:SS.sss. */
    USER_TIMECODE_MILLISECONDS = 3,
    /** Seconds only. */
    USER_TIMECODE_SECONDS_ONLY = 4,
    /**
     * Private (not exposed as generic choices) options.
     * milliseconds for sub-frames, SubRip format- HH:MM:SS,sss.
     */
    USER_TIMECODE_SUBRIP       = 100,
};

enum eUserpref_Translation_Flags
{
    USER_TR_TOOLTIPS            = (1 << 0),
    USER_TR_IFACE               = (1 << 1),
    USER_TR_REPORTS             = (1 << 2),
    USER_TR_UNUSED_3            = (1 << 3), /* cleared */
    USER_TR_UNUSED_4            = (1 << 4), /* cleared */
    USER_DOTRANSLATE_DEPRECATED = (1 << 5), /* Deprecated in 2.83. */
    USER_TR_UNUSED_6            = (1 << 6), /* cleared */
    USER_TR_UNUSED_7            = (1 << 7), /* cleared */
    USER_TR_NEWDATANAME         = (1 << 8),
};

enum eColorPicker_Types
{
    USER_CP_CIRCLE_HSV = 0,
    USER_CP_SQUARE_SV  = 1,
    USER_CP_SQUARE_HS  = 2,
    USER_CP_SQUARE_HV  = 3,
    USER_CP_CIRCLE_HSL = 4,
};

struct UserDef
{
    DNA_DEFINE_CXX_METHODS(UserDef)

    int   versionfile = 0, subversionfile = 0;

    int   flag = (USER_AUTOSAVE | USER_TOOLTIPS | USER_RELPATHS | USER_RELEASECONFIRM | USER_SCRIPT_AUTOEXEC_DISAVKE |
                USER_NONEGFRAMES | USER_FILECOMPRESS | USER_HIDE_DOT_DATABLOCK);

    char  pref_flag                            = USER_PREF_FLAG_SAVE;
    char  savetime                             = 2;
    char  mouse_emulate_3_button_modifier      = 0;

    char  trackpad_scroll_direction            = 0;
    /**  length. */
    char  tempdir[/*FILE_MAXDIR*/ 768] = "";
    char  fontdir[/*FILE_MAXDIR*/ 768] = "//";
    char  renderdir[/*FILE_MAX*/ 1024] = "//";
    /* EXR cache path */
    char  render_cachedir[/*FILE_MAXDIR*/ 768] = "";
    char  textudir[/*FILE_MAXDIR*/ 768] = "//";

    short v2d_min_gridsize                     = 45;
    /** #eTimecodeStyles, style of time-code display. */
    short timecode_style                       = USER_TIMECODE_MINIMAL;

    short versions                             = 1;

    short dbl_click_time                       = 350;

    char  _pad0[2]                             = {};

    /** Space around each area. Inter-editor gap width. */
    char  border_width                         = 2;

    float ui_scale                             = 1.0;

    int   ui_line_width                        = 0;
    /** Runtime, full DPI divided by `pixelsize`. */
    int   dpi                                  = 0;
    /** Runtime multiplier to scale UI elements. Use macro UI_SCALE_FAC instead of this. */
    float scale_factor                         = 0;
    /** Runtime, `1.0 / scale_factor` */
    float inv_scale_factor                     = 0;

    float pixelsize                            = 1;

    short transopts            = USER_TR_TOOLTIPS | USER_TR_IFACE | USER_TR_REPORTS | USER_TR_NEWDATANAME;

    // TODO: implement these ? maybe for user settings later
    // ListBaseT<bTheme>  themes   = {nullptr, nullptr};
    // ListBaseT<uiFont>  uifonts  = {nullptr, nullptr};
    // ListBaseT<uiStyle> uistyles = {nullptr, nullptr};
    //   ListBaseT<struct wmKeyMap> user_keymaps = {nullptr, nullptr};
    //   ListBaseT<struct wmKeyConfigPref> user_keyconfig_prefs = {nullptr, nullptr};
    //   ListBaseT<bAddon> addons = {nullptr, nullptr};
    //   ListBaseT<bPathCompare> autoexec_paths = {nullptr, nullptr};

    char  keyconfigstr[64]     = "Vektor";

    /** Index of the asset library being edited in the Preferences UI. */
    short active_asset_library = 0;

    short rvisize              = 25;
    /** Rotating view icon brightness. */
    short rvibright            = 8;
    /** Maximum number of recently used files to remember. */
    short recent_files         = 200;
    /** Milliseconds to spend spinning the view. */
    short smooth_viewtx        = 200;
    short glreslimit           = 0;
    /** #eColorPicker_Types. */
    short color_picker_type    = USER_CP_CIRCLE_HSV;

    char  _pad11[4]            = {};
};

// dna layer type.h
enum
{
    VIEW_LAYER_RENDER                 = (1 << 0),
    /* VIEW_LAYER_DEPRECATED  = (1 << 1), */
    VIEW_LAYER_FREESTYLE              = (1 << 2),
    VIEW_LAYER_OUT_OF_SYNC            = (1 << 3),
    VIEW_LAYER_HAS_EXPORT_COLLECTIONS = (1 << 4),
};

struct Base
{
    struct Base *               next = nullptr, *prev = nullptr;

    struct Object*              object                 = nullptr;

    /* Pointer to an original base. Is initialized for evaluated view layer.
     * NOTE: Only allowed to be accessed from within active dependency graph. */
    struct Base*                base_orig              = nullptr;

    DNA_DEPRECATED unsigned int lay                    = 0;
    /* Final flags, including both accumulated collection flags and object's
     * restriction flags. */
    short                       flag                   = 0;
    /* Flags which are based on the collections flags evaluation, does not
     * include flags from object's restrictions. */
    short                       flag_from_collection   = 0;
    short                       flag_legacy            = 0;
    unsigned short              local_view_bits        = 0;
    unsigned short              local_collections_bits = 0;
    char                        _pad1[2]               = {};
};

struct ViewLayer
{
    struct ViewLayer * next = nullptr, *prev = nullptr;
    char               name[/*MAX_NAME*/ 64] = "";
    short              flag                  = VIEW_LAYER_RENDER | VIEW_LAYER_FREESTYLE;
    char               _pad[6]               = {};
    ListBaseT<Base>    object_bases          = {nullptr, nullptr};
    /** Default allocated now. */
    struct SceneStats* stats                 = nullptr;
    struct Base*       basact                = nullptr;
};

// VLO read file .h
enum eBlenFileType
{
    VKENFILETYPE_VKEND = 1,
    // VKENFILETYPE_PUB = 2,     /* UNUSED */
    // VKENFILETYPE_RUNTIME = 3, /* UNUSED */
};

// vlo read file .h
struct BlendFileReadParams
{
    uint skip_flags : 3; /* #eBLOReadSkip */
    uint is_startup : 1;
    uint is_factory_settings : 1;

    /** Whether we are reading the memfile for an undo or a redo. */
    int  undo_direction; /* #eUndoStepDir */
};

// implmenent this in readfile . h

struct BlendFileData
    : NonCopyable
    , NonMovable
{
    Main*         main                        = nullptr;
    UserDef*      user                        = nullptr;

    int           fileflags                   = 0;
    int           globalf                     = 0;
    /** Typically the actual filepath of the read blend-file, except when recovering
     * save-on-exit/autosave files. In the latter case, it will be the path of the file that
     * generated the auto-saved one being recovered.
     *
     * NOTE: Currently expected to be the same path as #BlendFileData.filepath. */
    char          filepath[/*FILE_MAX*/ 1024] = {};

    ViewLayer*    cur_view_layer              = nullptr;

    eBlenFileType type                        = eBlenFileType(0);
};

BlendFileData* VKE_blendfile_read(const char*                filepath,
                                  const BlendFileReadParams* params,
                                  BlendFileReadReport*       reports);

void           BLO_blendfiledata_free(BlendFileData* bfd);

BlendFileData* VKE_blendfile_read_from_memory(const void*                file_buf,
                                              int                        file_buf_size,
                                              const BlendFileReadParams* params,
                                              ReportList*                reports);

// vke read file .h
BlendFileData* BLO_read_from_memory(const void*  mem,
                                    int          memsize,
                                    eBLOReadSkip skip_flags,
                                    ReportList*  reports);

void           VKE_blendfile_read_make_empty(vkContext* C);
UserDef*       VKE_blendfile_userdef_from_defaults();
void           VKE_vektor_userdef_data_set_and_free(UserDef* userdef);

} // namespace vektor