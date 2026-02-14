

#pragma once

#include "DNA_defs.h"
#include "DNA_id_enum.h"
#include "DNA_listBase.h"

#include "../vklib/VKE_assert.h"

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace vektor
{

namespace bke
{
namespace id
{
struct ID_Runtime;
}
} // namespace bke
namespace bke
{
struct PreviewImageRuntime;
}
namespace bke
{
namespace idprop
{
struct IDPropertyGroupChildrenSet;
}
} // namespace bke
namespace bke
{
namespace library
{
struct LibraryRuntime;
}
} // namespace bke

struct FileData;
struct GHash;
struct ID;
struct Library;
struct PackedFile;
struct UniqueName_Map;

struct IDPropertyUIData
{

    char* description = nullptr;

    int   rna_subtype = 0;

    char  _pad[4]     = {};
};

struct IDPropertyUIDataEnumItem
{

    char* identifier  = nullptr;

    char* name        = nullptr;

    char* description = nullptr;

    int   value       = 0;

    int   icon        = 0;
};

struct IDPropertyUIDataInt
{
    IDPropertyUIData          base;
    int*                      default_array     = nullptr;
    int                       default_array_len = 0;

    int                       min               = 0;
    int                       max               = 0;
    int                       soft_min          = 0;
    int                       soft_max          = 0;
    int                       step              = 0;
    int                       default_value     = 0;

    int                       enum_items_num    = 0;
    IDPropertyUIDataEnumItem* enum_items        = nullptr;
};

struct IDPropertyUIDataBool
{
    IDPropertyUIData base;
    int8_t*          default_array     = nullptr;
    int              default_array_len = 0;
    char             _pad[3]           = {};

    int8_t           default_value     = 0;
};

struct IDPropertyUIDataFloat
{
    IDPropertyUIData base;
    double*          default_array     = nullptr;
    int              default_array_len = 0;
    char             _pad[4]           = {};

    float            step              = 0;
    int              precision         = 0;

    double           min               = 0;
    double           max               = 0;
    double           soft_min          = 0;
    double           soft_max          = 0;
    double           default_value     = 0;
};

struct IDPropertyUIDataString
{
    IDPropertyUIData base;
    char*            default_value = nullptr;
};

struct IDPropertyUIDataID
{
    IDPropertyUIData base;

    short            id_type = 0;
    char             _pad[6] = {};
};

struct IDPropertyData
{
    void*                                    pointer      = nullptr;

    bke::idprop::IDPropertyGroupChildrenSet* children_map = nullptr;

    int                                      val = 0, val2 = 0;
};

struct IDProperty
{
    struct IDProperty *next = nullptr, *prev = nullptr;

    char               type     = 0;

    char               subtype  = 0;

    short              flag     = 0;
    char               name[64] = "";

    char               _pad0[4] = {};

    IDPropertyData     data;

    int                len      = 0;

    int                totallen = 0;

    IDPropertyUIData*  ui_data  = nullptr;
};

#define MAX_IDPROP_NAME 64
#define DEFAULT_ALLOC_FOR_NULL_STRINGS 64

struct IDOverrideLibraryPropertyOperation
{
    struct IDOverrideLibraryPropertyOperation *next = nullptr, *prev = nullptr;

    short                                      operation               = 0;
    short                                      flag                    = 0;

    short                                      tag                     = 0;
    char                                       _pad0[2]                = {};

    char*                                      subitem_reference_name  = nullptr;
    char*                                      subitem_local_name      = nullptr;
    int                                        subitem_reference_index = 0;
    int                                        subitem_local_index     = 0;

    struct ID*                                 subitem_reference_id    = nullptr;
    struct ID*                                 subitem_local_id        = nullptr;
};

enum
{

    LIBOVERRIDE_OP_NOOP          = 0,

    LIBOVERRIDE_OP_REPLACE       = 1,

    LIBOVERRIDE_OP_ADD           = 101,

    LIBOVERRIDE_OP_SUBTRACT      = 102,

    LIBOVERRIDE_OP_MULTIPLY      = 103,

    LIBOVERRIDE_OP_INSERT_AFTER  = 201,
    LIBOVERRIDE_OP_INSERT_BEFORE = 202,

};

enum
{

    LIBOVERRIDE_OP_FLAG_MANDATORY                 = 1 << 0,

    LIBOVERRIDE_OP_FLAG_LOCKED                    = 1 << 1,

    LIBOVERRIDE_OP_FLAG_IDPOINTER_MATCH_REFERENCE = 1 << 8,

    LIBOVERRIDE_OP_FLAG_IDPOINTER_ITEM_USE_ID     = 1 << 9,
};

struct IDOverrideLibraryProperty
{
    struct IDOverrideLibraryProperty *next = nullptr, *prev = nullptr;

    char*                             rna_path      = nullptr;

    short                             tag           = 0;
    char                              _pad[2]       = {};

    unsigned int                      rna_prop_type = 0;
};

enum
{

    LIBOVERRIDE_PROP_OP_TAG_UNUSED    = 1 << 0,

    LIBOVERRIDE_PROP_TAG_NEEDS_RETORE = 1 << 1,
};

#
#
struct IDOverrideLibraryRuntime
{
    struct GHash* rna_path_to_override_properties = nullptr;
    uint16_t      tag                             = 0;
};

enum
{

    LIBOVERRIDE_TAG_NEEDS_RELOAD              = 1 << 0,

    LIBOVERRIDE_TAG_NEEDS_RESTORE             = 1 << 1,

    LIBOVERRIDE_TAG_RESYNC_ISOLATED_FROM_ROOT = 1 << 2,

    LIBOVERRIDE_TAG_NEED_RESYNC_ORIGINAL      = 1 << 3,
};

struct IDOverrideLibrary
{

    struct ID*                reference      = nullptr;

    struct ID*                hierarchy_root = nullptr;

    IDOverrideLibraryRuntime* runtime        = nullptr;

    unsigned int              flag           = 0;
    char                      _pad_1[4]      = {};
};

enum
{

    LIBOVERRIDE_FLAG_NO_HIERARCHY   = 1 << 0,

    LIBOVERRIDE_FLAG_SYSTEM_DEFINED = 1 << 1,
};

#define MAX_ID_NAME 258

enum
{

    ID_REMAP_IS_LINKED_DIRECT    = 1 << 0,

    ID_REMAP_IS_USER_ONE_SKIPPED = 1 << 1,
};

struct IDHash
{
    char data[16] = "";

#ifdef __cplusplus
    uint64_t                hash() const { return *reinterpret_cast<const uint64_t*>(this->data); }

    static constexpr IDHash get_null() { return {}; }
    bool                    is_null() const { return *this == IDHash::get_null(); }

    friend bool             operator==(const IDHash& a,
                           const IDHash& b)
    {
        return memcmp(a.data, b.data, sizeof(a.data)) == 0;
    }

    friend bool operator!=(const IDHash& a,
                           const IDHash& b)
    {
        return !(a == b);
    }

#endif
};

struct ID
{

    void *                       next = nullptr, *prev = nullptr;
    struct ID*                   newid                  = nullptr;

    struct Library*              lib                    = nullptr;

    struct AssetMetaData*        asset_data             = nullptr;

    char                         name[258]              = "";

    short                        flag                   = 0;

    int                          tag                    = 0;
    int                          us                     = 0;
    int                          icon_id                = 0;
    unsigned int                 recalc                 = 0;

    unsigned int                 recalc_up_to_undo_push = 0;
    unsigned int                 recalc_after_undo_push = 0;

    unsigned int                 session_uid            = 0;

    IDHash                       deep_hash;

    IDProperty*                  properties             = nullptr;

    IDProperty*                  system_properties      = nullptr;

    void*                        _pad1                  = nullptr;

    IDOverrideLibrary*           override_library       = nullptr;

    struct ID*                   orig_id                = nullptr;

    void*                        py_instance            = nullptr;

    struct LibraryWeakReference* library_weak_reference = nullptr;

    bke::id::ID_Runtime*         runtime                = nullptr;
};

struct Library
{
#ifdef __cplusplus

    static constexpr ID_Type id_type = ID_LI;
#endif

    ID                            id;

    char                          filepath[1024]         = "";

    uint16_t                      flag                   = 0;

    uint16_t                      undo_runtime_tag       = 0;
    char                          _pad[4]                = {};

    struct Library*               archive_parent_library = nullptr;

    struct PackedFile*            packedfile             = nullptr;

    bke::library::LibraryRuntime* runtime                = nullptr;

    void*                         _pad2                  = nullptr;
};

enum LibraryFlag
{

    LIBRARY_FLAG_IS_ARCHIVE = 1 << 0,
};

struct LibraryWeakReference
{

    char library_filepath[1024] = "";

    char library_id_name[258]   = "";

    char _pad[2]                = {};
};

enum ePreviewImage_Flag
{
    PRV_CHANGED     = (1 << 0),

    PRV_USER_EDITED = (1 << 1),

    PRV_RENDERING   = (1 << 2),
};

enum
{

    PRV_TAG_DEFFERED_RENDERING = (1 << 1),

    PRV_TAG_DEFFERED_DELETE    = (1 << 2),

    PRV_TAG_DEFFERED_INVALID   = (1 << 3),
};

struct PreviewImage
{

    unsigned int              w[2]                 = {};
    unsigned int              h[2]                 = {};
    short                     flag[2]              = {};
    short                     changed_timestamp[2] = {};
    unsigned int*             rect[2]              = {};

    bke::PreviewImageRuntime* runtime              = nullptr;
};

#define ID_FAKE_USERS(id) ((id_cast<const ID*>(id)->flag & ID_FLAG_FAKEUSER) ? 1 : 0)

#define ID_EXTRA_USERS(id) (id_cast<const ID*>(id)->tag & ID_TAG_EXTRAUSER ? 1 : 0)

#define ID_EXTRA_REAL_USERS(id) (id_cast<const ID*>(id)->tag & ID_TAG_EXTRAUSER_SET ? 1 : 0)

#define ID_REAL_USERS(id) (id_cast<const ID*>(id)->us - ID_FAKE_USERS(id))

#define ID_REFCOUNTING_USERS(id) (ID_REAL_USERS(id) - ID_EXTRA_REAL_USERS(id))

#define ID_CHECK_UNDO(id) (!ELEM(GS((id)->name), ID_SCR, ID_WM, ID_WS, ID_BR))

#define ID_BLEND_PATH(_bmain, _id)                                                                                     \
    ((_id)->lib ? BKE_main_blendfile_path_from_library(*(_id)->lib) : BKE_main_blendfile_path((_bmain)))
#define ID_BLEND_PATH_FROM_GLOBAL(_id)                                                                                 \
    ((_id)->lib ? BKE_main_blendfile_path_from_library(*(_id)->lib) : BKE_main_blendfile_path_from_global())

#define ID_MISSING(_id) ((id_cast<const ID*>(_id)->tag & ID_TAG_MISSING) != 0)

#define ID_IS_LINKED(_id) (id_cast<const ID*>(_id)->lib != NULL)

#define ID_IS_PACKED(_id) (ID_IS_LINKED(_id) && ((_id)->flag & ID_FLAG_LINKED_AND_PACKED))

#define ID_TYPE_SUPPORTS_ASSET_EDITABLE(id_type) ELEM(id_type, ID_BR, ID_TE, ID_NT, ID_IM, ID_PC, ID_MA)

#define ID_IS_EDITABLE(_id)                                                                                            \
    ((id_cast<const ID*>(_id)->lib == NULL) ||                                                                         \
     ((id_cast<const ID*>(_id)->lib->runtime->tag & LIBRARY_ASSET_EDITABLE) &&                                         \
      ID_TYPE_SUPPORTS_ASSET_EDITABLE(GS(id_cast<const ID*>(_id)->name))))

#define ID_IS_OVERRIDABLE_LIBRARY_HIERARCHY(_id)                                                                       \
    (ID_IS_LINKED(_id) && !ID_MISSING(_id) &&                                                                          \
     (BKE_idtype_get_info_from_id(id_cast<const ID*>(_id))->flags & IDTYPE_FLAGS_NO_LIBLINKING) == 0 &&                \
     !ELEM(GS((id_cast<const ID*>(_id))->name), ID_SCE))
#define ID_IS_OVERRIDABLE_LIBRARY(_id)                                                                                 \
    (ID_IS_OVERRIDABLE_LIBRARY_HIERARCHY((_id)) && (id_cast<const ID*>(_id)->tag & ID_TAG_EXTERN) != 0)

#define ID_IS_OVERRIDE_LIBRARY_REAL(_id)                                                                               \
    (id_cast<const ID*>(_id)->override_library != NULL && id_cast<const ID*>(_id)->override_library->reference != NULL)

#define ID_IS_OVERRIDE_LIBRARY_VIRTUAL(_id) ((id_cast<const ID*>(_id)->flag & ID_FLAG_EMBEDDED_DATA_LIB_OVERRIDE) != 0)

#define ID_IS_OVERRIDE_LIBRARY(_id) (ID_IS_OVERRIDE_LIBRARY_REAL(_id) || ID_IS_OVERRIDE_LIBRARY_VIRTUAL(_id))

#define ID_IS_OVERRIDE_LIBRARY_HIERARCHY_ROOT(_id)                                                                     \
    (!ID_IS_OVERRIDE_LIBRARY_REAL(_id) || (id_cast<ID*>(_id))->override_library->hierarchy_root == (id_cast<ID*>(_id)))

#define ID_IS_ASSET(_id) (id_cast<const ID*>(_id)->asset_data != NULL)

#define ID_TYPE_USE_COPY_ON_EVAL(_id_type)                                                                             \
    (!ELEM(_id_type, ID_LI, ID_SCR, ID_VF, ID_BR, ID_WM, ID_PAL, ID_PC, ID_WS, ID_IM))

#define ID_TYPE_SUPPORTS_PARAMS_WITHOUT_COW(id_type) ELEM(id_type, ID_ME)

#define ID_TYPE_IS_DEPRECATED(id_type) false

#ifdef GS
#undef GS
#endif
#define GS(a) (CHECK_TYPE_ANY(a, char*, const char*), (ID_Type)(*((const short*)(a))))

#define ID_NEW_SET(_id, _idn)                                                                                          \
    (((id_cast<ID*>)(_id))->newid = (id_cast<ID*>)(_idn), ((id_cast<ID*>)(_id))->newid->tag |= ID_TAG_NEW,             \
     ((id_cast<ID*>)(_id))->newid)
#define ID_NEW_REMAP(a)                                                                                                \
    if ((a) && (a)->id.newid)                                                                                          \
    {                                                                                                                  \
        *(void**)&(a) = (a)->id.newid;                                                                                 \
    }                                                                                                                  \
    ((void)0)

enum
{

    ID_FLAG_FAKEUSER                     = 1 << 9,

    ID_FLAG_EMBEDDED_DATA                = 1 << 10,

    ID_FLAG_INDIRECT_WEAK_LINK           = 1 << 11,

    ID_FLAG_EMBEDDED_DATA_LIB_OVERRIDE   = 1 << 12,

    ID_FLAG_LIB_OVERRIDE_RESYNC_LEFTOVER = 1 << 13,

    ID_FLAG_CLIPBOARD_MARK               = 1 << 14,

    ID_FLAG_LINKED_AND_PACKED            = 1 << 15,
};

enum
{

    ID_TAG_LOCAL                        = 0,

    ID_TAG_EXTERN                       = 1 << 0,

    ID_TAG_INDIRECT                     = 1 << 1,

    ID_TAG_RUNTIME                      = 1 << 2,

    ID_TAG_MISSING                      = 1 << 3,

    ID_TAG_EXTRAUSER                    = 1 << 4,

    ID_TAG_EXTRAUSER_SET                = 1 << 5,

    ID_TAG_LIBOVERRIDE_REFOK            = 1 << 6,

    ID_TAG_LIBOVERRIDE_AUTOREFRESH      = 1 << 7,

    ID_TAG_LIBOVERRIDE_NEED_RESYNC      = 1 << 8,

    ID_TAG_NEW                          = 1 << 12,

    ID_TAG_PRE_EXISTING                 = 1 << 13,

    ID_TAG_UNDO_OLD_ID_REUSED_UNCHANGED = 1 << 17,

    ID_TAG_UNDO_OLD_ID_REUSED_NOUNDO    = 1 << 18,

    ID_TAG_UNDO_OLD_ID_REREAD_IN_PLACE  = 1 << 19,

    ID_TAG_TEMP_MAIN                    = 1 << 20,

    ID_TAG_NO_MAIN                      = 1 << 21,

    ID_TAG_LOCALIZED                    = 1 << 22,

    ID_TAG_COPIED_ON_EVAL               = 1 << 23,

    ID_TAG_COPIED_ON_EVAL_FINAL_RESULT  = 1 << 24,

    ID_TAG_NO_USER_REFCOUNT             = 1 << 25,

    ID_TAG_NOT_ALLOCATED                = 1 << 26,

    ID_TAG_DOIT                         = 1u << 31,
};

#define ID_TAG_KEEP_ON_UNDO                                                                                            \
    (ID_TAG_EXTRAUSER | ID_TAG_MISSING | ID_TAG_RUNTIME | ID_TAG_LOCAL | ID_TAG_EXTERN | ID_TAG_INDIRECT)

enum IDRecalcFlag
{

    ID_RECALC_TRANSFORM          = (1 << 0),

    ID_RECALC_GEOMETRY           = (1 << 1),

    ID_RECALC_ANIMATION          = (1 << 2),

    ID_RECALC_PSYS_REDO          = (1 << 3),

    ID_RECALC_PSYS_RESET         = (1 << 4),

    ID_RECALC_PSYS_CHILD         = (1 << 5),

    ID_RECALC_PSYS_PHYS          = (1 << 6),

    ID_RECALC_SHADING            = (1 << 7),

    ID_RECALC_SELECT             = (1 << 9),

    ID_RECALC_BASE_FLAGS         = (1 << 10),
    ID_RECALC_POINT_CACHE        = (1 << 11),

    ID_RECALC_EDITORS            = (1 << 12),

    ID_RECALC_SYNC_TO_EVAL       = (1 << 13),

    ID_RECALC_SEQUENCER_STRIPS   = (1 << 14),

    ID_RECALC_FRAME_CHANGE       = (1 << 15),

    ID_RECALC_AUDIO_FPS          = (1 << 16),
    ID_RECALC_AUDIO_VOLUME       = (1 << 17),
    ID_RECALC_AUDIO_MUTE         = (1 << 18),
    ID_RECALC_AUDIO_LISTENER     = (1 << 19),

    ID_RECALC_AUDIO              = (1 << 20),

    ID_RECALC_PARAMETERS         = (1 << 21),

    ID_RECALC_SOURCE             = (1 << 23),

    ID_RECALC_TAG_FOR_UNDO       = (1 << 24),

    ID_RECALC_NTREE_OUTPUT       = (1 << 25),

    ID_RECALC_HIERARCHY          = (1 << 26),

    ID_RECALC_PROVISION_27       = (1 << 27),
    ID_RECALC_PROVISION_28       = (1 << 28),
    ID_RECALC_PROVISION_29       = (1 << 29),
    ID_RECALC_PROVISION_30       = (1 << 30),
    ID_RECALC_PROVISION_31       = (1u << 31),

    ID_RECALC_ANIMATION_NO_FLUSH = ID_RECALC_SYNC_TO_EVAL,

    ID_RECALC_GEOMETRY_ALL_MODES = ID_RECALC_GEOMETRY | ID_RECALC_SYNC_TO_EVAL,

    ID_RECALC_ALL                = (0xffffffff),

    ID_RECALC_PSYS_ALL = (ID_RECALC_PSYS_REDO | ID_RECALC_PSYS_RESET | ID_RECALC_PSYS_CHILD | ID_RECALC_PSYS_PHYS),

};

#define FILTER_ID_AC (1ULL << 0)
#define FILTER_ID_AR (1ULL << 1)
#define FILTER_ID_BR (1ULL << 2)
#define FILTER_ID_CA (1ULL << 3)
#define FILTER_ID_CU_LEGACY (1ULL << 4)
#define FILTER_ID_GD_LEGACY (1ULL << 5)
#define FILTER_ID_GR (1ULL << 6)
#define FILTER_ID_IM (1ULL << 7)
#define FILTER_ID_LA (1ULL << 8)
#define FILTER_ID_LS (1ULL << 9)
#define FILTER_ID_LT (1ULL << 10)
#define FILTER_ID_MA (1ULL << 11)
#define FILTER_ID_MB (1ULL << 12)
#define FILTER_ID_MC (1ULL << 13)
#define FILTER_ID_ME (1ULL << 14)
#define FILTER_ID_MSK (1ULL << 15)
#define FILTER_ID_NT (1ULL << 16)
#define FILTER_ID_OB (1ULL << 17)
#define FILTER_ID_PAL (1ULL << 18)
#define FILTER_ID_PC (1ULL << 19)
#define FILTER_ID_SCE (1ULL << 20)
#define FILTER_ID_SPK (1ULL << 21)
#define FILTER_ID_SO (1ULL << 22)
#define FILTER_ID_TE (1ULL << 23)
#define FILTER_ID_TXT (1ULL << 24)
#define FILTER_ID_VF (1ULL << 25)
#define FILTER_ID_WO (1ULL << 26)
#define FILTER_ID_PA (1ULL << 27)
#define FILTER_ID_CF (1ULL << 28)
#define FILTER_ID_WS (1ULL << 29)
#define FILTER_ID_LP (1ULL << 31)
#define FILTER_ID_CV (1ULL << 32)
#define FILTER_ID_PT (1ULL << 33)
#define FILTER_ID_VO (1ULL << 34)
#define FILTER_ID_SIM (1ULL << 35)
#define FILTER_ID_KE (1ULL << 36)
#define FILTER_ID_SCR (1ULL << 37)
#define FILTER_ID_WM (1ULL << 38)
#define FILTER_ID_LI (1ULL << 39)
#define FILTER_ID_GP (1ULL << 40)
#define FILTER_ID_IP (1ULL << 41)

#define FILTER_ID_ALL                                                                                                  \
    (FILTER_ID_AC | FILTER_ID_AR | FILTER_ID_BR | FILTER_ID_CA | FILTER_ID_CU_LEGACY | FILTER_ID_GD_LEGACY |           \
     FILTER_ID_GR | FILTER_ID_IM | FILTER_ID_LA | FILTER_ID_LS | FILTER_ID_LT | FILTER_ID_MA | FILTER_ID_MB |          \
     FILTER_ID_MC | FILTER_ID_ME | FILTER_ID_MSK | FILTER_ID_NT | FILTER_ID_OB | FILTER_ID_PA | FILTER_ID_PAL |        \
     FILTER_ID_PC | FILTER_ID_SCE | FILTER_ID_SPK | FILTER_ID_SO | FILTER_ID_TE | FILTER_ID_TXT | FILTER_ID_VF |       \
     FILTER_ID_WO | FILTER_ID_CF | FILTER_ID_WS | FILTER_ID_LP | FILTER_ID_CV | FILTER_ID_PT | FILTER_ID_VO |          \
     FILTER_ID_SIM | FILTER_ID_KE | FILTER_ID_SCR | FILTER_ID_WM | FILTER_ID_LI | FILTER_ID_GP | FILTER_ID_IP)

enum eID_Index
{

    INDEX_ID_LI = 0,

    INDEX_ID_AC,

    INDEX_ID_GD_LEGACY,

    INDEX_ID_NT,

    INDEX_ID_VF,
    INDEX_ID_TXT,
    INDEX_ID_SO,

    INDEX_ID_MSK,
    INDEX_ID_IM,
    INDEX_ID_MC,

    INDEX_ID_TE,
    INDEX_ID_MA,
    INDEX_ID_LS,
    INDEX_ID_WO,

    INDEX_ID_CF,
    INDEX_ID_PA,

    INDEX_ID_KE,

    INDEX_ID_AR,
    INDEX_ID_ME,
    INDEX_ID_CU_LEGACY,
    INDEX_ID_MB,
    INDEX_ID_CV,
    INDEX_ID_PT,
    INDEX_ID_VO,
    INDEX_ID_LT,
    INDEX_ID_LA,
    INDEX_ID_CA,
    INDEX_ID_SPK,
    INDEX_ID_LP,
    INDEX_ID_GP,

    INDEX_ID_OB,
    INDEX_ID_GR,

    INDEX_ID_PAL,
    INDEX_ID_PC,
    INDEX_ID_BR,

    INDEX_ID_SCE,

    INDEX_ID_SCR,
    INDEX_ID_WS,
    INDEX_ID_WM,

    INDEX_ID_NULL,
};

#define INDEX_ID_MAX (INDEX_ID_NULL + 1)

} // namespace vektor
