#pragma once

#include "../dna/DNA_vec_types.h"
#include "../kernel/intern/VK_report.hh"
#include "wm_types.h"

#include "../vklib/VKE_enum_flags.hh"
#include "../vklib/VKE_main.hh"

namespace vektor
{
struct wmEventHandler;
struct StructRNA;
struct vkContext;
struct wmWindowManager;
extern wmWindowManager* G_WM;
struct vkContext;
struct ReportList;
struct bScreen;
struct rcti;

enum eWM_CapabilitiesFlag
{
    /** Ability to warp the cursor (set its location). */
    WM_CAPABILITY_CURSOR_WARP                   = (1 << 0),
    /**
     * Window position access, support for the following.
     * - Getting window positions.
     * - Setting window positions.
     * - Setting positions for new windows.
     *
     * Currently there is no need to distinguish between these different cases
     * so a single flag is used.
     *
     * When omitted, it isn't possible to know where windows are located in relation to each other.
     * Operations such as applying events from one window to another or detecting the non-active
     * window under the cursor are not supported.
     */
    WM_CAPABILITY_WINDOW_POSITION               = (1 << 1),
    /**
     * The windowing system supports a separate primary clipboard
     * (typically set when interactively selecting text).
     */
    WM_CAPABILITY_CLIPBOARD_PRIMARY             = (1 << 2),
    /**
     * Reading from the back-buffer is supported.
     */
    WM_CAPABILITY_GPU_FRONT_BUFFER_READ         = (1 << 3),
    /** Ability to copy/paste system clipboard images. */
    WM_CAPABILITY_CLIPBOARD_IMAGE               = (1 << 4),
    /** Ability to sample a color outside of Blender windows. */
    WM_CAPABILITY_DESKTOP_SAMPLE                = (1 << 5),
    /** Support for IME input methods. */
    WM_CAPABILITY_INPUT_IME                     = (1 << 6),
    /** Trackpad physical scroll detection. */
    WM_CAPABILITY_TRACKPAD_PHYSICAL_DIRECTION   = (1 << 7),
    /** Support for window decoration styles. */
    WM_CAPABILITY_WINDOW_DECORATION_STYLES      = (1 << 8),
    /** Support for the "Hyper" modifier key. */
    WM_CAPABILITY_KEYBOARD_HYPER_KEY            = (1 << 9),
    /** Support for RGBA Cursors. */
    WM_CAPABILITY_CURSOR_RGBA                   = (1 << 10),
    /** Support on demand cursor generation. */
    WM_CAPABILITY_CURSOR_GENERATOR              = (1 << 11),
    /** Ability to save/restore windows among multiple monitors. */
    WM_CAPABILITY_MULTIMONITOR_PLACEMENT        = (1 << 12),
    /** Support for the window to show a file-path (otherwise include in the title text). */
    WM_CAPABILITY_WINDOW_PATH                   = (1 << 13),
    /** Support for window server side decorations (SSD). */
    WM_CAPABILITY_WINDOW_DECORATION_SERVER_SIDE = (1 << 14),
    /** The initial value, indicates the value needs to be set by inspecting GHOST. */
    WM_CAPABILITY_INITIALIZED                   = (1u << 31),
};
ENUM_OPERATORS(eWM_CapabilitiesFlag)

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
bool        WM_file_read(vkContext*  C,
                         const char* filepath,
                         const bool  use_scripts_autoexec_check,
                         ReportList* reports);

void        WM_file_autosave_init(wmWindowManager* wm);

bool        WM_file_recover_last_session(vkContext*  C,
                                         const bool  use_scripts_autoexec_check,
                                         ReportList* reports);

wmWindow*   WM_window_open(vkContext*       C,
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

void        wm_window_clear_drawable(wmWindowManager* wm);

void        WM_event_timer_remove(wmWindowManager* wm,
                                  wmWindow*        win,
                                  wmTimer*         timer);

void        WM_event_timer_free_data(wmTimer* timer);

void        WM_event_remove_handlers_by_area(ListBaseT<wmEventHandler>* handlers,
                                             const ScrArea*             area);

void        WM_event_remove_handlers(vkContext*                 C,
                                     ListBaseT<wmEventHandler>* handlers);

void        wm_window_free(vkContext*       C,
                           wmWindowManager* wm,
                           wmWindow*        win);

void*       ctx_wm_python_context_get(const vkContext* C,
                                      const char*      member,
                                      const StructRNA* member_type,
                                      void*            fall_through);

void        WM_window_set_active_screen(wmWindow*  win,
                                        WorkSpace* workspace,
                                        bScreen*   screen) ATTR_NONNULL(1);
} // namespace vektor
