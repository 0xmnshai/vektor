#pragma once

// using wm types .h for scrarea will change to screen VK later
#include "../../runtime/vklib/VKE_string_ref.hh"
#include "../../runtime/vklib/VKE_workspace.hh" // IWYU pragma: keep

using namespace vektor::vklib;

namespace vektor
{
struct wmWindow;
struct wmOperator;
struct ScrArea;
struct ScratchArea;
struct vkContext;
struct bScreen;
struct WorkSpace;
struct WorkSpaceLayout;
struct Main;

ScrArea*         ED_fileselect_handler_area_find(wmWindow*   win,
                                                 wmOperator* op);
void             ED_area_exit(vkContext* C,
                              ScrArea*   area);

void             ED_screen_exit(vkContext* C,
                                wmWindow*  win,
                                bScreen*   screen);

WorkSpaceLayout* ED_workspace_layout_duplicate(Main*                  bmain,
                                               WorkSpace*             workspace,
                                               const WorkSpaceLayout* layout_old,
                                               wmWindow*              win);

bool             VKE_screen_is_fullscreen_area(const bScreen* screen);

} // namespace vektor

StringRefNull ED_area_name(const vektor::ScrArea* area);
