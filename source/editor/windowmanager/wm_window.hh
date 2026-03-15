
#pragma once

#include "../../source/runtime/lib/intern/context.hh"

namespace vektor {

struct Main;
struct wmWindow;
struct wmWindowManager;

/**
 * Handle to a window's runtime state.
 */
struct WindowRuntime;

enum eWindowAlignment {
  WIN_ALIGN_NONE = 0,
  WIN_ALIGN_LEFT,
  WIN_ALIGN_RIGHT,
  WIN_ALIGN_TOP,
  WIN_ALIGN_BOTTOM,
};

/* *************** Public API ************** */

wmWindow *wm_window_new(const Main *vkmain, wmWindowManager *wm, wmWindow *parent, bool dialog);

wmWindow *WM_window_open(lib::vkContext *C,
                         const char *title,
                         int space_type,
                         bool toplevel,
                         bool dialog,
                         bool temp,
                         eWindowAlignment alignment,
                         void (*area_setup_fn)(void *user_data),
                         void *area_setup_user_data);

void WM_window_title_refresh(wmWindowManager *wm, wmWindow *win);

bool WM_window_is_temp_screen(const wmWindow *win);

}  // namespace vektor
