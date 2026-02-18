#include <fmt/format.h>
#include <string>

#include "../../editor/include/ED_screen.hh"
#include "../dna/DNA_listBase.h"
#include "../dna/DNA_space_enums.h"
#include "../translation/VKT_translation.hh"
#include "wm_api.h"
#include "wm_system.h"

#include "../vklib/VKE_build_config.h"
#include "../vklib/VKE_main.hh"
#include "../vklib/VKE_path_util.hh"
#include "../vklib/VKE_string_utf8.h"
#include "../vklib/VKE_version.hh"

#include "../../intern/file_system/fileops.hh"
#include "../kernel/intern/VKE_wm_runtime.h"

#include "../../intern/glfw/GLFW_System.hh"
#include "GLFW_window.hh"

namespace vektor
{

std::string wm_window_title_text(wmWindowManager*                                wm,
                                 wmWindow*                                       win,
                                 std::optional<std::function<void(const char*)>> window_filepath_fn)
{
    if (win->parent || WM_window_is_temp_screen(win))
    {
        bScreen*   screen    = WM_window_get_active_screen(win);
        const bool is_single = screen && VKE_listbase_is_single(&screen->areabase);

        ScrArea*   area      = (screen) ? static_cast<ScrArea*>(screen->areabase.first) : nullptr;

        if (is_single && area &&
            area->spacetype != eSpace_Type::SPACE_EMPTY) // need to change from wm_types.h to DNA_space_enums.h
        {
            return IFACE_(ED_area_name(area).c_str());
        }

        return "Vektor";
    }

    const char* filepath_as_bytes = VKE_main_vktfile_path_from_global();
    char        _filepath_utf8_buf[FILE_MAX];

    const char* filepath =
        (OS_MAC || OS_WINDOWS)
            ? filepath_as_bytes
            : VKE_str_utf8_invalid_substitute_if_needed(filepath_as_bytes, strlen(filepath_as_bytes), '?',
                                                        _filepath_utf8_buf, sizeof(_filepath_utf8_buf));

    const char* filename                = VKE_path_basename(filepath);
    const bool  has_filepath            = filepath[0] != '\0';
    const bool  native_filepath_display = (window_filepath_fn != std::nullopt);

    if (native_filepath_display)
    {
        (*window_filepath_fn)(filepath_as_bytes);
    }
    const bool  include_filepath = has_filepath && (filepath != filename) && !native_filepath_display;

    std::string win_title        = wm->file_saved ? "" : "* ";

    if (include_filepath)
    {
        const size_t filename_no_ext_len = VKE_path_extension_or_end(filename) - filename;
        win_title.append(filename, filename_no_ext_len);
    }
    else if (has_filepath)
    {
        win_title.append(filename);
    }
    else
    {
#ifdef __APPLE__
        win_title.append(IFACE_("Untitled"));
#else
        win_title.append(IFACE_("(Unsaved)"));
#endif
    }

    if (include_filepath)
    {
        bool add_filepath = true;
        if ((OS_MAC || OS_WINDOWS) == 0)
        {
            if (const char* home_dir = FS_dir_home())
            {
                size_t home_dir_len = strlen(home_dir);
                /* Strip trailing slash (if it exists). */
                while (home_dir_len && home_dir[home_dir_len - 1] == SEP)
                {
                    home_dir_len--;
                }
                if ((home_dir_len > 0) && VKE_path_ncmp(home_dir, filepath_as_bytes, home_dir_len) == 0)
                {
                    if (filepath_as_bytes[home_dir_len] == SEP)
                    {
                        win_title.append(fmt::format(" [~{}]", filepath + home_dir_len));
                        add_filepath = false;
                    }
                }
            }
        }
        if (add_filepath)
        {
            win_title.append(fmt::format(" [{}]", filepath));
        }
    }

    win_title.append(fmt::format(" - Vektor {}", VKE_vektor_version_string()));

    return win_title;
};

void wm_window_title_state_refresh(wmWindowManager* wm,
                                   wmWindow*        win)
{
    GLFW_Window* window = static_cast<GLFW_Window*>(win->runtime->glfw_win);
    window->set_modified_state(!wm->file_saved);
}

void WM_window_title_refresh(wmWindowManager* wm,
                             wmWindow*        win)
{
    if (win->runtime->glfw_win == nullptr)
    {
        return;
    }

    GLFW_Window* window             = static_cast<GLFW_Window*>(win->runtime->glfw_win);
    auto         window_filepath_fn = (WM_capabilities_flag() & WM_CAPABILITY_WINDOW_PATH)
                                          ? std::optional([&window](const char* filepath) { window->set_file_path(filepath); })
                                          : std::nullopt;
    std::string  win_title          = wm_window_title_text(wm, win, window_filepath_fn);
    window->set_title(win_title.c_str());
    wm_window_title_state_refresh(wm, win);
}

// --------------------------------------------------------------------
// Vektor Window Implementation
// --------------------------------------------------------------------

class VektorWindow : public GLFW_Window
{
public:
    VektorWindow(uint32_t                  width,
                 uint32_t                  height,
                 GLFW_TWindowState         state,
                 const GLFW_ContextParams& context_params,
                 const bool                exclusive = false)
        : GLFW_Window(width,
                      height,
                      state,
                      context_params,
                      exclusive)
    {
    }

    // GLFW_Window implements create_window, but defines it as virtual or pure virtual in base?
    // In GLFW_Window.hh: virtual GLFW_IWindow* create_window(...) = 0;
    // In GLFW_Window.cc: GLFW_IWindow* GLFW_Window::create_window(...) { ... }
    // So GLFW_Window::create_window is the implementation we want to use.
    // However, since it is pure virtual in the base (if GLFW_Window is abstract? No, GLFW_Window inherits from
    // GLFW_IWindow). GLFW_IWindow HAS create_window as pure virtual. GLFW_Window declares it overrides it? In
    // GLFW_Window.hh: virtual GLFW_IWindow* create_window(...) = 0; -> WAIT. If GLFW_Window.hh says = 0, then
    // GLFW_Window IS abstract and cannot be instantiated directly, even if it has an implementation body in .cc (which
    // can be called by subclasses). So we MUST override it in VektorWindow.

    GLFW_IWindow* create_window(const char*         title,
                                int32_t             left,
                                int32_t             top,
                                uint32_t            width,
                                uint32_t            height,
                                GLFW_TWindowState   state,
                                GLFW_GPUSettings    gpu_settings,
                                const bool          exclusive     = false,
                                const bool          is_dialog     = false,
                                const GLFW_IWindow* parent_window = nullptr) override
    {
        return GLFW_Window::create_window(title, left, top, width, height, state, gpu_settings, exclusive, is_dialog,
                                          parent_window);
    }

    // Check other pure virtuals?
    // GLFW_IWindow: init(), set_title(), ...
    // GLFW_Window implements all of them?
    // GLFW_Window.hh: virtual void init() const override; -> ok
};

// --------------------------------------------------------------------
// Internal Helpers
// --------------------------------------------------------------------

static void wm_window_ghostwindow_add(wmWindowManager* wm,
                                      const char*      title,
                                      wmWindow*        win,
                                      bool             is_dialog)
{
    GLFW_ContextParams context_params;
    context_params.ctx_major =
        3; // Fixed from major_version based on typical naming, verifying in next step via tool if it fails again
    context_params.ctx_minor       = 3;                                    // Fixed from minor_version
    context_params.profile         = GLFW_ContextParams::CtxProfile::Core; // Guessing enum, will check

    // Instantiate our concrete window class
    auto*            vektor_window = new VektorWindow(win->sizex, win->sizey, GLFW_kWindowStateNormal, context_params);

    // Create the actual OS window
    GLFW_GPUSettings gpu_settings;
    vektor_window->create_window(title, win->posx, win->posy, win->sizex, win->sizey, GLFW_kWindowStateNormal,
                                 gpu_settings, false, is_dialog, nullptr);

    win->runtime->glfw_win = vektor_window;

    // Register with system
    if (GLFW_ISystem::get_system())
    {
        if (auto* sys = dynamic_cast<GLFW_System*>(GLFW_ISystem::get_system()))
        {
            sys->get_window_manager()->add_window(vektor_window);
        }
    }
}

static void wm_window_ghostwindow_ensure(wmWindowManager* wm,
                                         wmWindow*        win,
                                         bool             is_dialog)
{
    if (win->runtime->glfw_win == nullptr)
    {
        wm_window_ghostwindow_add(wm, win->title.c_str(), win, is_dialog);
    }
}

// --------------------------------------------------------------------
// Implementation
// --------------------------------------------------------------------

wmWindow* wm_window_new(const Main*      vkmain,
                        wmWindowManager* wm,
                        wmWindow*        parent,
                        bool             dialog)
{
    auto win     = new wmWindow();
    win->runtime = new WindowRuntime();
    // Default size/pos from constructor

    if (wm->get_window())
    {
        // Append to end of list
        wmWindow* last = wm->get_window().get();
        while (last->next)
            last = last->next;
        last->next = win;
        win->prev  = last;
    }
    // Else: we can't easily set the head of the list because it's a shared_ptr in wmWindowManager
    // and we don't have access to set it from here (private member, only getter).
    // However, if get_window() is null, something is wrong or we are efficiently the first window
    // but can't attach to the manager?
    // G_WM constructor creates a default window_.
    // So get_window() should not be null.
    // But if we are creating a NEW window (e.g. second window), we append.

    return win;
}

wmWindow* WM_window_open(bContext*        C,
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
                         void* area_setup_user_data)
{
    wmWindowManager* wm  = G_WM;

    // Create new window structure
    wmWindow*        win = wm_window_new(nullptr, wm, nullptr, dialog);

    if (title)
        win->title = title;

    if (rect_unscaled)
    {
        win->posx  = rect_unscaled->xmin;
        win->posy  = rect_unscaled->ymin;
        win->sizex = rect_unscaled->xmax - rect_unscaled->xmin;
        win->sizey = rect_unscaled->ymax - rect_unscaled->ymin;
    }

    // Ensure ghost window (OS window)
    wm_window_ghostwindow_ensure(wm, win, dialog);

    return win;
}

bool WM_file_read(bContext*   C,
                  const char* filepath,
                  const bool  use_scripts_autoexec_check,
                  ReportList* reports)
{
    // Stub
    return false;
}

void WM_file_autosave_init(wmWindowManager* wm)
{
    // Stub
}

bool WM_file_recover_last_session(bContext*   C,
                                  const bool  use_scripts_autoexec_check,
                                  ReportList* reports)
{
    // Stub
    return false;
}

bool WM_window_is_temp_screen(const wmWindow* win)
{
    return win->is_temp_screen;
}

bScreen* WM_window_get_active_screen(const wmWindow* win)
{
    // Check if the window has a screen
    if (!win->screen)
    {
        return nullptr;
    }
    return win->screen.get();
}
} // namespace vektor
