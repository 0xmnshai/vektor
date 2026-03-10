/* SPDX-FileCopyrightText: 2026 Vektor Engine. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup wm
 */

#pragma once

namespace vektor {

struct vkContext;
struct Main;
struct wmWindow;
struct wmWindowManager;
struct bScreen;
struct ScrArea;
struct rcti;
struct WorkSpace;

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

wmWindow *wm_window_new(const Main *vkmain,
                        wmWindowManager *wm,
                        wmWindow *parent,
                        bool dialog);

wmWindow *WM_window_open(vkContext *C,
                         const char *title,
                         const rcti *rect_unscaled,
                         int space_type,
                         bool toplevel,
                         bool dialog,
                         bool temp,
                         eWindowAlignment alignment,
                         void (*area_setup_fn)(bScreen *screen,
                                               ScrArea *area,
                                               void *user_data),
                         void *area_setup_user_data);

void WM_window_title_refresh(wmWindowManager *wm, wmWindow *win);

bool WM_window_is_temp_screen(const wmWindow *win);
bScreen *WM_window_get_active_screen(const wmWindow *win);

}  // namespace vektor
