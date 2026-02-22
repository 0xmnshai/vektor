#include "../../creator/creator_global.hh"
#include "wm_api.h"

#include "../../intern/glfw/GLFW_window.hh"
#include "wm_files.h"

namespace vektor
{
GLFW_Window* glfw_window;

void         WM_init(vkContext*   vkC,
                     int          argc,
                     const char** argv)
{

    // everything will be init here

    // call glfw init here ...
    // in the above init we will read .vektor file
    // execute wm_homefile_read_ex
    // implement WM_check ( wm.cc )
    // implement wm_window_ghostwindows_ensure ( wm_window.cc )
    // implement wm_window_ghostwindow_add ( wm_window.cc )
    // here inside the above function we will call _system->createWindow(...)

    // vektor . cc or main .cc must call : vektor_version_init ( vektor_version_init )
    // which is called within VKE_vektor_globals_init ( to be defined if not in vektor.h)
    // VKE_vektor_globals_init is defined in main .cc
    // which must be called in creator.cc

    // later check screen_draw.cc in vektor
    // need to implment this : WM_window_open

    if (!creator::G.background)
    {
        glfw_window = new GLFW_Window();
        glfw_window->init();
    }

    // WM_file_read(vkC, nullptr, true, nullptr); // we will call this at createor_args.cc

    wm_homefile_read_ex(vkC, nullptr, nullptr, nullptr);
}

void WM_exit() {}

} // namespace vektor