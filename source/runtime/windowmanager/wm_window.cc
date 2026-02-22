#include <fmt/format.h>
#include <string>

#include "../dna/DNA_listBase.h"
#include "../dna/DNA_space_enums.h"
#include "../translation/VKT_translation.hh"
#include "wm_api.h"

#include "../vklib/VKE_build_config.h"
#include "../vklib/VKE_main.hh"
#include "../vklib/VKE_path_util.hh"
#include "../vklib/VKE_string_utf8.h"
#include "../vklib/VKE_version.hh"

#include "../../intern/file_system/fileops.hh"
#include "../../intern/gaurdalloc/MEM_gaurdalloc.hh"
#include "GLFW_window.hh"

struct wmKeyConfig;
#include "../../editor/include/ED_screen.hh" // IWYU pragma: keep
#include "../../intern/clog/COG_log.hh"
#include "../dna/DNA_windowmanager_types.h"
#include "../vklib/VKE_list_base.h"
#include "../vklib/VKE_listbase_iterator.hh" // IWYU pragma: keep
#include "wm_system.h"

static CLG_LogRef LOG = {"wm.window"};

#include "../vklib/VKE_context.h"

#include "../../intern/gaurdalloc/mallocn.cc"
#include "../rna/RNA_internal_types.h"

#include "../python/bpy_extern.h"

namespace vektor
{

bool        screen_temp_region_exists(const ARegion* region);
void        wm_event_free_handler(wmEventHandler* handler);

void        WM_cursor_grab_disable(wmWindow* win,
                                   void*     customdata);
void        wm_operator_free_for_fileselect(wmOperator* op);
void        WM_operator_free(wmOperator* op);

std::string wm_window_title_text(wmWindowManager*                                wm,
                                 wmWindow*                                       win,
                                 std::optional<std::function<void(const char*)>> window_filepath_fn)
{
    if (win->parent || WM_window_is_temp_screen(win))
    {
        bScreen*   screen    = WM_window_get_active_screen(win);
        const bool is_single = screen && VKE_listbase_is_single(&screen->areabase);

        ScrArea*   area      = (screen) ? static_cast<ScrArea*>(screen->areabase.first) : nullptr;

        if (is_single && area && area->spacetype != SPACE_EMPTY) // need to change from wm_types.h to DNA_space_enums.h
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
                                          ? std::optional([&window](const char* filepath) { window->set_path(filepath); })
                                          : std::nullopt;
    std::string  win_title          = wm_window_title_text(wm, win, window_filepath_fn);
    window->set_title(win_title.c_str());
    wm_window_title_state_refresh(wm, win);
}

static int find_free_winid(wmWindowManager* wm)
{
    int id = 1;

    for (wmWindow& win : wm->windows)
    {
        if (id <= win.winid)
        {
            id = win.winid + 1;
        }
    }
    return id;
}

wmWindow* wm_window_new(const Main*      vkmain,
                        wmWindowManager* wm,
                        wmWindow*        parent,
                        bool             dialog)
{
    wmWindow* win = MEM_new<wmWindow>("window");

    VKE_addtail(&wm->windows, win);
    win->winid   = find_free_winid(wm);

    win->parent  = (!dialog && parent && parent->parent) ? parent->parent : parent;
    win->runtime = MEM_new<WindowRuntime>(__func__);

    return win;
}

wmWindow* WM_window_open(vkContext*       C,
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

    return win;
}

void WM_file_autosave_init(wmWindowManager* wm)
{
    // Stub
}

bool WM_file_recover_last_session(vkContext*  C,
                                  const bool  use_scripts_autoexec_check,
                                  ReportList* reports)
{
    // Stub
    return false;
}

bool WM_window_is_temp_screen(const wmWindow* win)
{
    return false; // win->is_temp_screen;
}

bScreen* WM_window_get_active_screen(const wmWindow* win)
{
    // Check if the window has a screen
    if (!win->screen)
    {
        return nullptr;
    }
    return win->screen;
}

void wm_window_clear_drawable(wmWindowManager* wm)
{
    if (wm->runtime->windrawable)
    {
        wm->runtime->windrawable = nullptr;
    }
}

void WM_event_timer_free_data(wmTimer* timer)
{
    if (timer->customdata != nullptr && (timer->flags & WM_TIMER_NO_FREE_CUSTOM_DATA) == 0)
    {
        MEM_delete_void(timer->customdata);
        timer->customdata = nullptr;
    }
}

void wm_window_free(vkContext*       C,
                    wmWindowManager* wm,
                    wmWindow*        win)
{
    if (C)
    {
        WM_event_remove_handlers(C, reinterpret_cast<ListBaseT<wmEventHandler>*>(&win->handlers));
    }
}

wmWindowManager* CTX_wm_manager(const vkContext* C)
{
    return C->wm.manager;
}

// context.cc

void* CTX_py_dict_get(const vkContext* C)
{
    return C->data.py_context;
}

void* ctx_wm_python_context_get(const vkContext* C,
                                const char*      member,
                                const StructRNA* member_type,
                                void*            fall_through)
{
    void* return_data  = nullptr;
    bool  found_member = false;

    if (UNLIKELY(CTX_py_dict_get(C)))
    {
        bContextDataResult result{};
        if (BPY_context_member_get(const_cast<vkContext*>(C), member, &result))
        {
            found_member = true;

            if (result.ptr.data)
            {
                if (RNA_struct_is_a(result.ptr.type, member_type))
                {
                    return_data = result.ptr.data;
                }
                else
                {
                    CLOG_WARN(&LOG, "PyContext '%s' is a '%s', expected a '%s'", member,
                              result.ptr.type->identifier.c_str(), member_type->identifier.c_str());
                }
            }

            /* Log context member access directly without storing a copy. */
            ctx_member_log_access(C, member, result, CTX_RESULT_OK);
        }
    }
    if (!found_member)
    {
        bContextDataResult fallback_result{};
        fallback_result.ptr.data = fall_through;
        fallback_result.ptr.type = const_cast<StructRNA*>(member_type); /* Use the expected RNA type */
        fallback_result.type     = ContextDataType::Pointer;
        return_data              = fall_through;

        /* Log fallback context member access. */
        ctx_member_log_access(C, member, fallback_result, CTX_RESULT_MEMBER_NOT_FOUND);
    }

    /* Don't allow UI context access from non-main threads. */
    if (!BLI_thread_is_main())
    {
        return nullptr;
    }

    return return_data;
}

wmWindow* CTX_wm_window(const vkContext* C)
{
    StructRNA* RNA_Window = nullptr;
    return static_cast<wmWindow*>(ctx_wm_python_context_get(C, "window", RNA_Window, C->wm.window));
}

void WM_event_remove_handlers(vkContext*                 C,
                              ListBaseT<wmEventHandler>* handlers)
{
    wmWindowManager* wm = CTX_wm_manager(C);

    /* C is zero on freeing database, modal handlers then already were freed. */
    while (wmEventHandler* handler_base = static_cast<wmEventHandler*>(BLI_pophead(handlers)))
    {
        if (handler_base->type == WM_HANDLER_TYPE_OP)
        {
            wmEventHandler_Op* handler = reinterpret_cast<wmEventHandler_Op*>(handler_base);

            if (handler->op)
            {
                wmWindow* win = CTX_wm_window(C);

                if (handler->is_fileselect)
                {
                    /* Exit File Browsers referring to this handler/operator. */
                    for (wmWindow& temp_win : wm->windows)
                    {
                        ScrArea* file_area = ED_fileselect_handler_area_find(&temp_win, handler->op);
                        if (!file_area)
                        {
                            continue;
                        }
                        ED_area_exit(C, file_area);
                    }
                }

                if (handler->op->type->cancel)
                {
                    ScrArea* area   = CTX_wm_area(C);
                    ARegion* region = CTX_wm_region(C);

                    // wm_handler_op_context(C, handler, win->runtime->eventstate);

                    if (handler->op->type->flag & OPTYPE_UNDO)
                    {
                        wm->op_undo_depth++;
                    }

                    handler->op->type->cancel(C, handler->op);

                    if (handler->op->type->flag & OPTYPE_UNDO)
                    {
                        wm->op_undo_depth--;
                    }

                    CTX_wm_area_set(C, area);
                    CTX_wm_region_set(C, region);
                }

                WM_cursor_grab_disable(win, nullptr);

                if (handler->is_fileselect)
                {
                    wm_operator_free_for_fileselect(handler->op);
                }
                else
                {
                    WM_operator_free(handler->op);
                }
            }
        }
        else if (handler_base->type == WM_HANDLER_TYPE_UI)
        {
            wmEventHandler_UI* handler = reinterpret_cast<wmEventHandler_UI*>(handler_base);

            if (handler->remove_fn)
            {
                ScrArea* area_prev         = CTX_wm_area(C);
                ARegion* region_prev       = CTX_wm_region(C);
                ARegion* region_popup_prev = CTX_wm_region_popup(C);

                if (handler->context.area)
                {
                    CTX_wm_area_set(C, handler->context.area);
                }
                if (handler->context.region)
                {
                    CTX_wm_region_set(C, handler->context.region);
                }
                if (handler->context.region_popup)
                {
                    // BLI_assert(screen_temp_region_exists(handler->context.region_popup));
                    CTX_wm_region_popup_set(C, handler->context.region_popup);
                }

                handler->remove_fn(C, handler->user_data);

                /* Currently we don't have a practical way to check if this region
                 * was a temporary region created by `handler`, so do a full lookup. */
                if (region_popup_prev && !screen_temp_region_exists(region_popup_prev))
                {
                    region_popup_prev = nullptr;
                }

                CTX_wm_area_set(C, area_prev);
                CTX_wm_region_set(C, region_prev);
                CTX_wm_region_popup_set(C, region_popup_prev);
            }
        }

        wm_event_free_handler(handler_base);
    }
}

void WM_window_set_active_screen(wmWindow*  win,
                                 WorkSpace* workspace,
                                 bScreen*   screen)
{
    VKE_workspace_active_screen_set(win->workspace_hook, win->winid, workspace, screen);
}

} // namespace vektor
