#include "wm_api.h"

namespace vektor
{

void WM_init(vkContext*   vkC,
             int          argc,
             const char** argv)
{
    // call glfw init here ...
    // in the above init we will read .vektor file
    // execute wm_homefile_read_ex
    // implement WM_check ( wm.cc )
    // implement wm_window_ghostwindows_ensure ( wm_window.cc )
    // implement wm_window_ghostwindow_add ( wm_window.cc )
    // here inside the above function we will call _system->createWindow(...)

    // vektor . cc or main .cc must call : blender_version_init ( vektor_version_init )
    // which is called within BKE_blender_globals_init ( to be defined if not in vektor.h)
    // BKE_blender_globals_init is defined in main .cc
    // which must be called in creator.cc 

    // later check screen_draw.cc in blender
    // need to implment this : WM_window_open
}

void WM_exit() {}

} // namespace vektor