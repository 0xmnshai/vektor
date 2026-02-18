#pragma once

#include "../dna/DNA_vec_types.h"
#include "../kernel/intern/VK_report.hh"
#include "../vklib/VKE_compiler_attrs.h"
#include "wm_types.h"

#include "../vklib/VKE_main.hh"

namespace vektor
{
struct vkContext;
struct wmWindowManager;
extern wmWindowManager* G_WM;
struct bContext;
struct ReportList;
struct bScreen;
struct rcti;

enum eWM_CapabilitiesFlag
{
    WM_CAPABILITY_WINDOW_POSITION = (1 << 1),
    WM_CAPABILITY_WINDOW_PATH     = (1 << 13),

    WM_CAPABILITY_INITIALIZED     = (1u << 31),
};

eWM_CapabilitiesFlag WM_capabilities_flag();

enum eWindowAlignment
{
    WIN_ALIGN_ABSOLUTE = 0,
    WIN_ALIGN_LOCATION_CENTER,
    WIN_ALIGN_PARENT_CENTER,
};

// defined in wm_init_exit.cc
void        WM_init(vkContext*   vkC,
                    int          argc,
                    const char** argv);

void        WM_exit();

// rest are defined in wm_window.cc
std::string wm_window_title_text(wmWindowManager*                                wm,
                                 wmWindow*                                       win,
                                 std::optional<std::function<void(const char*)>> window_filepath_fn);

void        wm_window_title_state_refresh(wmWindowManager* wm,
                                          wmWindow*        win);

void        WM_window_title_refresh(wmWindowManager* wm,
                                    wmWindow*        win);

wmWindow*   wm_window_new(const Main*      vkmain,
                          wmWindowManager* wm,
                          wmWindow*        parent,
                          bool             dialog);

// LATER, LAST SESSION CAN BE READ FROM CACHE TO RE-STORE THE
bool        WM_file_read(bContext*   C,
                         const char* filepath,
                         const bool  use_scripts_autoexec_check,
                         ReportList* reports);

void        WM_file_autosave_init(wmWindowManager* wm);

bool        WM_file_recover_last_session(bContext*   C,
                                         const bool  use_scripts_autoexec_check,
                                         ReportList* reports);

wmWindow*   WM_window_open(bContext*        C,
                           const char*      title,
                           const rcti*      rect_unscaled,
                           int              space_type,
                           bool             toplevel,
                           bool             dialog,
                           bool             temp,
                           eWindowAlignment alignment,
                           void (*area_setup_fn)(bScreen* screen,
                                               ScrArea* area,
                                               void*    user_data),
                           void* area_setup_user_data);

bool        WM_window_is_temp_screen(const wmWindow* win) ATTR_WARN_UNUSED_RESULT;

bScreen*    WM_window_get_active_screen(const wmWindow* win) ATTR_NONNULL() ATTR_WARN_UNUSED_RESULT;

} // namespace vektor
