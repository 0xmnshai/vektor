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
    // 
}

void WM_exit() {}

} // namespace vektor