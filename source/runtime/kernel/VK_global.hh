
#pragma once

#include "../dna/DNA_listBase.h"

namespace vektor
{
namespace kernel
{

struct Main;
struct RecentFile;

struct Global
{

    Main*                 main;

    Main*                 pr_main;

    char                  filepath_last_image[1024];
    char                  filepath_last_library[1024];
    char                  filepath_last_blend[1024];

    ListBaseT<RecentFile> recent_files;

    bool                  is_break;

    bool                  background;

    bool                  factory_startup;

    short                 moving;

    bool                  is_rendering;

    short                 debug_value;

    int                   f;

    struct
    {

        int   level;

        void* file;
    } log;

    int  debug;

    bool randomize_geometry_element_order;

    int  fileflags;

    char autoexec_fail[200];

    char gpu_debug_scope_name[100];

    char gpu_debug_shader_source_name[100];

    bool profile_gpu;
};

enum
{
    G_FLAG_RENDER_VIEWPORT                = (1 << 0),
    G_FLAG_PICKSEL                        = (1 << 2),
    G_FLAG_EVENT_SIMULATE                 = (1 << 3),
    G_FLAG_USERPREF_NO_SAVE_ON_EXIT       = (1 << 4),

    G_FLAG_INTERNET_ALLOW                 = (1 << 10),

    G_FLAG_INTERNET_OVERRIDE_PREF_ONLINE  = (1 << 11),
    G_FLAG_INTERNET_OVERRIDE_PREF_OFFLINE = (1 << 12),

    G_FLAG_SCRIPT_AUTOEXEC                = (1 << 13),
    G_FLAG_SCRIPT_OVERRIDE_PREF           = (1 << 14),
    G_FLAG_SCRIPT_AUTOEXEC_FAIL           = (1 << 15),
    G_FLAG_SCRIPT_AUTOEXEC_FAIL_QUIET     = (1 << 16),

    G_FLAG_GPU_BACKEND_FALLBACK           = (1 << 17),
    G_FLAG_GPU_BACKEND_FALLBACK_QUIET     = (1 << 18),

};

#define G_FLAG_INTERNET_OVERRIDE_PREF_ANY (G_FLAG_INTERNET_OVERRIDE_PREF_ONLINE | G_FLAG_INTERNET_OVERRIDE_PREF_OFFLINE)

#define G_FLAG_ALL_RUNTIME                                                                                             \
    (G_FLAG_SCRIPT_AUTOEXEC | G_FLAG_SCRIPT_OVERRIDE_PREF | G_FLAG_INTERNET_ALLOW |                                    \
     G_FLAG_INTERNET_OVERRIDE_PREF_ONLINE | G_FLAG_INTERNET_OVERRIDE_PREF_OFFLINE | G_FLAG_EVENT_SIMULATE |            \
     G_FLAG_USERPREF_NO_SAVE_ON_EXIT | G_FLAG_GPU_BACKEND_FALLBACK | G_FLAG_GPU_BACKEND_FALLBACK_QUIET |               \
                                                                                                                       \
     G_FLAG_SCRIPT_AUTOEXEC_FAIL_QUIET)

#define G_FLAG_ALL_READFILE 0

enum
{
    G_DEBUG                             = (1 << 0),
    G_DEBUG_PYTHON                      = (1 << 2),
    G_DEBUG_EVENTS                      = (1 << 3),
    G_DEBUG_HANDLERS                    = (1 << 4),
    G_DEBUG_WM                          = (1 << 5),
    G_DEBUG_JOBS                        = (1 << 6),
    G_DEBUG_FREESTYLE                   = (1 << 7),
    G_DEBUG_DEPSGRAPH_BUILD             = (1 << 8),
    G_DEBUG_DEPSGRAPH_EVAL              = (1 << 9),
    G_DEBUG_DEPSGRAPH_TAG               = (1 << 10),
    G_DEBUG_DEPSGRAPH_TIME              = (1 << 11),
    G_DEBUG_DEPSGRAPH_NO_THREADS        = (1 << 12),
    G_DEBUG_DEPSGRAPH_PRETTY            = (1 << 13),
    G_DEBUG_DEPSGRAPH_UID               = (1 << 14),
    G_DEBUG_DEPSGRAPH                   = (G_DEBUG_DEPSGRAPH_BUILD | G_DEBUG_DEPSGRAPH_EVAL | G_DEBUG_DEPSGRAPH_TAG |
                         G_DEBUG_DEPSGRAPH_TIME | G_DEBUG_DEPSGRAPH_UID),
    G_DEBUG_SIMDATA                     = (1 << 15),
    G_DEBUG_GPU                         = (1 << 16),
    G_DEBUG_IO                          = (1 << 17),
    G_DEBUG_GPU_FORCE_WORKAROUNDS       = (1 << 18),
    G_DEBUG_GPU_FORCE_VULKAN_LOCAL_READ = (1 << 19),
    G_DEBUG_GPU_COMPILE_SHADERS         = (1 << 20),
    G_DEBUG_GPU_RENDERDOC               = (1 << 21),
    G_DEBUG_GPU_SHADER_DEBUG_INFO       = (1 << 22),
    G_DEBUG_GPU_NO_TEXTURE_POOL         = (1 << 23),
    G_DEBUG_XR                          = (1 << 24),
    G_DEBUG_XR_TIME                     = (1 << 25),

    G_DEBUG_VEKTOR                       = (1 << 26),
    G_DEBUG_WINTAB                      = (1 << 27),

    G_DEBUG_GPU_SHADER_NO_PREPROCESSOR  = (1 << 28),
    G_DEBUG_GPU_SHADER_NO_DCE           = (1 << 29),
};

#define G_DEBUG_ALL                                                                                                    \
    (G_DEBUG | G_DEBUG_PYTHON | G_DEBUG_EVENTS | G_DEBUG_WM | G_DEBUG_JOBS | G_DEBUG_FREESTYLE | G_DEBUG_DEPSGRAPH |   \
     G_DEBUG_IO | G_DEBUG_VEKTOR | G_DEBUG_WINTAB)

enum
{
    G_FILE_AUTOPACK              = (1 << 0),
    G_FILE_COMPRESS              = (1 << 1),

    G_BACKGROUND_NO_DEPSGRAPH    = 1 << 2,

    G_LIBOVERRIDE_NO_AUTO_RESYNC = 1 << 3,

    G_FILE_NO_UI                 = (1 << 10),
    G_FILE_RECOVER_READ          = (1 << 23),
    G_FILE_RECOVER_WRITE         = (1 << 24),
    G_FILE_ASSET_EDIT_FILE       = (1 << 29),
};

#define G_FILE_FLAG_ALL_RUNTIME                                                                                        \
    (G_BACKGROUND_NO_DEPSGRAPH | G_LIBOVERRIDE_NO_AUTO_RESYNC | G_FILE_NO_UI | G_FILE_RECOVER_READ |                   \
     G_FILE_RECOVER_WRITE)

enum
{
    G_TRANSFORM_OBJ     = (1 << 0),
    G_TRANSFORM_EDIT    = (1 << 1),
    G_TRANSFORM_SEQ     = (1 << 2),
    G_TRANSFORM_FCURVES = (1 << 3),
    G_TRANSFORM_WM      = (1 << 4),
    G_TRANSFORM_CURSOR  = (1 << 5),
};

extern Global G;

#define G_MAIN (G).main

} // namespace kernel

} // namespace vektor