/* SPDX-FileCopyrightText: 2026 Vektor Engine. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup wm
 */

#include "wm_window.hh"

#include <fmt/format.h>
#include <functional>
#include <string>
#include <vector>

#include "../../intern/clog/CLG_log.h"
#include "../../intern/gaurdalloc/MEM_gaurdalloc.h"

namespace vektor {

struct WindowRuntime {
  void *glfw_win;
};

struct wmWindow {
  int winid;
  wmWindow *parent;
  WindowRuntime *runtime;
  const char *title;
  int posx, posy, sizex, sizey;
};

struct wmWindowManager {
  bool file_saved;
  std::vector<wmWindow *> windows;
};

CLG_LOGREF_DECLARE_GLOBAL(V_LOG, "wm.window");

std::string wm_window_title_text(
    wmWindowManager *wm,
    wmWindow *win,
    std::optional<std::function<void(const char *)>> window_filepath_fn)
{
  // Simplified version of the provided logic
  if (win->parent) {
    return "Vektor Tool";
  }

  const char *filepath = "Untitled.vkt";  // Placeholder
  std::string win_title = wm->file_saved ? "" : "* ";
  win_title.append(filepath);
  win_title.append(fmt::format(" - Vektor {}", "0.1.0"));

  if (window_filepath_fn) {
    (*window_filepath_fn)(filepath);
  }

  return win_title;
}

void WM_window_title_refresh(wmWindowManager *wm, wmWindow *win)
{
  if (!win->runtime || !win->runtime->glfw_win) {
    return;
  }

  // Assuming GLFW_Window wrapper or similar
  std::string title = wm_window_title_text(wm, win, std::nullopt);
  // glfwSetWindowTitle((GLFWwindow*)win->runtime->glfw_win, title.c_str());
  CLOG_INFO(V_LOG, "Window title refreshed: %s", title.c_str());
}

static int find_free_winid(wmWindowManager *wm)
{
  int id = 1;
  for (auto *win : wm->windows) {
    if (id <= win->winid) {
      id = win->winid + 1;
    }
  }
  return id;
}

wmWindow *wm_window_new(const Main * /*vkmain*/,
                        wmWindowManager *wm,
                        wmWindow *parent,
                        bool dialog)
{
  auto *win = mem::MEM_new<wmWindow>("window");

  wm->windows.push_back(win);
  win->winid = find_free_winid(wm);

  win->parent = (!dialog && parent && parent->parent) ? parent->parent : parent;
  win->runtime = mem::MEM_new<WindowRuntime>("WindowRuntime");

  return win;
}

wmWindow *WM_window_open(lib::vkContext * /*C*/,
                         const char *title,
                         int /*space_type*/,
                         bool /*toplevel*/,
                         bool dialog,
                         bool /*temp*/,
                         eWindowAlignment /*alignment*/,
                         void (* /*area_setup_fn*/)(void *user_data),
                         void * /*area_setup_user_data*/)
{
  // This would normally come from a global or context
  static wmWindowManager dummy_wm = {true, {}};

  wmWindow *win = wm_window_new(nullptr, &dummy_wm, nullptr, dialog);

  if (title) {
    win->title = title;
  }

  return win;
}

bool WM_window_is_temp_screen(const wmWindow * /*win*/)
{
  return false;
}
}  // namespace vektor
