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
#include "wm_window_glfw.h"

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
} // namespace vektor

// later check screen_draw.cc in blender
// need to implment this : WM_window_open