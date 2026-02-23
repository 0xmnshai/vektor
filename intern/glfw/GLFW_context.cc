#include <glad/glad.h>

#include "GLFW_context.hh"

namespace vektor
{
static GLFWwindow* s_sharedContext = nullptr;
static int         s_sharedCount   = 0;

static void        initClearGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

GLFW_TSuccess GLFW_Context::init_drawing_context()
{
    const bool needAlpha = false;

    glfwWindowHint(GLFW_OPENGL_PROFILE, context_profile_mask_);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_major_version_);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_minor_version_);

    if (context_flags_ & GLFW_OPENGL_DEBUG_CONTEXT)
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    }
    if (context_flags_ & GLFW_OPENGL_FORWARD_COMPAT)
    {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);

    if (needAlpha)
    {
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
    }
    else
    {
        glfwWindowHint(GLFW_ALPHA_BITS, 0); // Explicitly 0
    }
    if (context_params_.is_stereo_visual)
    {
        glfwWindowHint(GLFW_STEREO, GLFW_TRUE);
    }

    if (window_ == nullptr)
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        hidden_window_ = glfwCreateWindow(1, 1, "Offscreen Context Window", nullptr, s_sharedContext);
        window_        = hidden_window_;
    }

    if (window_ == nullptr)
    {
        return GLFW_kFailure;
    }

    if (!s_sharedContext)
    {
        s_sharedContext = window_;
    }

    s_sharedCount++;
    glfwMakeContextCurrent(window_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return GLFW_kFailure;
    }

    const GLFW_TVSyncModes vsync = context_params_.vsync;
    if (vsync != GLFW_kVSyncModeUnset)
    {
        glfwSwapInterval(int(vsync));
    }

    initClearGL();
    glfwSwapBuffers(window_);

    active_context_ = this;

    return GLFW_kSuccess;
}
} // namespace vektor