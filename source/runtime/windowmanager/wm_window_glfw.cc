
#include <iostream>

#include "GLFW/glfw3.h"

#include "../../intern/clog/COG_log.hh"
#include "wm_window_glfw.h"

CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_App,
                          "WindowManager");

namespace vektor
{
static std::shared_ptr<wmWindow> window_ = nullptr;

void                             GLFW_Window::init(int    argc,
                                                   char** argv)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }
}

void GLFW_Window::error_callback(int         error,
                                 const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void GLFW_Window::create_window(std::shared_ptr<wmWindow> window)
{
    window_ = window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to initialize GLFW");
        return;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    // GLFWwindow* window = glfwCreateWindow(1280, 720, "Vektor Style ImGui + Python", NULL, NULL);
}
} // namespace vektor