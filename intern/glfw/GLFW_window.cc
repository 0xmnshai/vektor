#include <glad/glad.h> // IWYU pragma: keep
#include <cstdint>

#include "../../intern/clog/COG_log.hh"
#include "GLFW/glfw3.h"
#include "GLFW_Event.hh"
#include "GLFW_ISystem.hh"
#include "GLFW_window.hh"

namespace vektor
{

CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_App,
                          "GLFW Window");

void GLFW_Window::error_callback(int         error,
                                 const char* description)
{
    CLOG_ERROR(CLG_LogRef_App, "GLFW Error %d: %s", error, description);
}

void GLFW_Window::key_callback(GLFWwindow* window,
                               int         key,
                               int         scancode,
                               int         action,
                               int         mods)
{
    GLFW_Window* glfw_window = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window));
    if (glfw_window)
    {
        GLFW_EventKeyData data;
        data.key             = key;
        data.scancode        = scancode;
        data.action          = action;
        data.mods            = mods;

        GLFW_TEventType type = (action == GLFW_RELEASE) ? GLFW_kEventKeyUp : GLFW_kEventKeyDown;
        // Basic mapping. Repeat is treated as down.

        auto event = std::make_unique<GLFW_Event>(type, glfw_window, GLFW_ISystem::get_system()->get_milli_seconds(),
                                                  &data, sizeof(data));
        GLFW_ISystem::get_system()->push_event(std::move(event));
    }
}

void GLFW_Window::mouse_button_callback(GLFWwindow* window,
                                        int         button,
                                        int         action,
                                        int         mods)
{
    GLFW_Window* glfw_window = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window));
    if (glfw_window)
    {
        GLFW_EventButtonData data;
        data.button          = button;
        data.action          = action;
        data.mods            = mods;

        GLFW_TEventType type = (action == GLFW_RELEASE) ? GLFW_kEventButtonUp : GLFW_kEventButtonDown;

        auto event = std::make_unique<GLFW_Event>(type, glfw_window, GLFW_ISystem::get_system()->get_milli_seconds(),
                                                  &data, sizeof(data));
        GLFW_ISystem::get_system()->push_event(std::move(event));
    }
}

void GLFW_Window::cursor_pos_callback(GLFWwindow* window,
                                      double      xpos,
                                      double      ypos)
{
    GLFW_Window* glfw_window = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window));
    if (glfw_window)
    {
        GLFW_EventCursorData data;
        data.x     = xpos;
        data.y     = ypos;

        auto event = std::make_unique<GLFW_Event>(GLFW_kEventCursorMove, glfw_window,
                                                  GLFW_ISystem::get_system()->get_milli_seconds(), &data, sizeof(data));
        GLFW_ISystem::get_system()->push_event(std::move(event));
    }
}

void GLFW_Window::scroll_callback(GLFWwindow* window,
                                  double      xoffset,
                                  double      yoffset)
{
    GLFW_Window* glfw_window = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window));
    if (glfw_window)
    {
        GLFW_EventScrollData data;
        data.xoffset = xoffset;
        data.yoffset = yoffset;

        auto event   = std::make_unique<GLFW_Event>(GLFW_kEventWheel, glfw_window,
                                                    GLFW_ISystem::get_system()->get_milli_seconds(), &data, sizeof(data));
        GLFW_ISystem::get_system()->push_event(std::move(event));
    }
}

void GLFW_Window::frame_buffer_size_callback(GLFWwindow* window,
                                             int         width,
                                             int         height)
{
    GLFW_Window* glfw_window = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window));
    if (glfw_window)
    {
        width_  = width;
        height_ = height;

        GLFW_EventWindowSizeData data;
        data.width  = width;
        data.height = height;

        auto event  = std::make_unique<GLFW_Event>(GLFW_kEventWindowSize, glfw_window,
                                                   GLFW_ISystem::get_system()->get_milli_seconds(), &data, sizeof(data));
        GLFW_ISystem::get_system()->push_event(std::move(event));
    }
}

GLFW_Window::GLFW_Window(uint32_t                  width,
                         uint32_t                  height,
                         GLFW_TWindowState         state,
                         const GLFW_ContextParams& context_params,
                         const bool                exclusive)
{
    user_data_ = nullptr;
    context_   = nullptr;
    os_window_ = nullptr;
    width_     = width;
    height_    = height;
    state_     = state;
    is_valid_  = false;
}

GLFW_Window::~GLFW_Window()
{
    // delete the glfw - opengl pointer
    glfwDestroyWindow((GLFWwindow*)os_window_);
    glfwTerminate();
};

void GLFW_Window::init() const
{
    if (!glfwInit())
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to initialize GLFW");
        return;
    }

    glfwSetErrorCallback(error_callback);
};

GLFW_IWindow* GLFW_Window::create_window(const char*         title,
                                         int32_t             left,
                                         int32_t             top,
                                         uint32_t            width,
                                         uint32_t            height,
                                         GLFW_TWindowState   state,
                                         GLFW_GPUSettings    gpu_settings,
                                         const bool          exclusive,
                                         const bool          is_dialog,
                                         const GLFW_IWindow* parent_window)
{
    os_window_ = glfwCreateWindow(width_, height_, title, NULL, NULL);

    set_title(title);

    if (os_window_ == NULL)
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to create GLFW window");
        return nullptr;
    }

    // Set user pointer so callbacks can retrieve the instance
    glfwSetWindowUserPointer((GLFWwindow*)os_window_, this);

    // Register callbacks
    glfwSetKeyCallback((GLFWwindow*)os_window_, key_callback);
    glfwSetMouseButtonCallback((GLFWwindow*)os_window_, mouse_button_callback);
    glfwSetCursorPosCallback((GLFWwindow*)os_window_, cursor_pos_callback);
    glfwSetScrollCallback((GLFWwindow*)os_window_, scroll_callback);
    glfwSetFramebufferSizeCallback((GLFWwindow*)os_window_, frame_buffer_size_callback);

    glfwMakeContextCurrent((GLFWwindow*)os_window_);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to initialize GLAD");
        return nullptr;
    }

    return this;
}

void GLFW_Window::set_title(const char* title)
{
    title_ = title;
    glfwSetWindowTitle((GLFWwindow*)get_os_window(), title_);
};

GLFW_TSuccess GLFW_Window::invalidate() const
{
    if (is_valid_ == false)
    {
        is_valid_ = true;
        glfwMakeContextCurrent((GLFWwindow*)os_window_);
    }
    return GLFW_kSuccess;
};

GLFW_TSuccess GLFW_Window::set_window_state(GLFW_TWindowState state)
{
    switch (state)
    {
        case GLFW_kWindowStateNormal:
            glfwRestoreWindow((GLFWwindow*)os_window_);
            break;
        case GLFW_kWindowStateMaximized:
            glfwMaximizeWindow((GLFWwindow*)os_window_);
            break;
        case GLFW_kWindowStateMinimized:
            glfwIconifyWindow((GLFWwindow*)os_window_);
            break;
        case GLFW_kWindowStateFullScreen:
            glfwSetWindowMonitor((GLFWwindow*)os_window_, glfwGetWindowMonitor((GLFWwindow*)os_window_), 0, 0, width_,
                                 height_, glfwGetVideoMode(glfwGetWindowMonitor((GLFWwindow*)os_window_))->refreshRate);
            break;
    }

    state_ = state;
    return GLFW_kSuccess;
}

GLFW_TWindowState GLFW_Window::get_window_state() const
{
    uint32_t flags = glfwGetWindowAttrib((GLFWwindow*)os_window_, GLFW_HOVERED);

    if (flags & GLFW_HOVERED)
        return GLFW_kWindowStateNormal;
    if (flags & GLFW_FOCUSED)
        return GLFW_kWindowStateMaximized;
    if (flags & GLFW_ICONIFIED)
        return GLFW_kWindowStateMinimized;
    if (flags & GLFW_VISIBLE)
        return GLFW_kWindowStateFullScreen;

    return GLFW_kWindowStateNormal;
}

void GLFW_Window::set_modified_state(const bool modified) const
{
    glfwSetWindowAttrib((GLFWwindow*)os_window_, GLFW_FOCUSED, modified);
}

void GLFW_Window::get_window_bounds(GLFW_Rect& bounds) const
{
    get_client_bounds(bounds);
}

void GLFW_Window::get_client_bounds(GLFW_Rect& bounds) const
{
    int x, y, width, height;
    glfwGetWindowPos((GLFWwindow*)os_window_, &x, &y);
    glfwGetWindowSize((GLFWwindow*)os_window_, &width, &height);
    bounds.l_ = x;
    bounds.t_ = y;
    bounds.r_ = width;
    bounds.b_ = height;
}

GLFW_TSuccess GLFW_Window::set_client_width(uint32_t width) const
{
    glfwSetWindowSize((GLFWwindow*)os_window_, width, height_);
    width_ = width;
    return GLFW_kSuccess;
};

GLFW_TSuccess GLFW_Window::set_client_height(uint32_t height) const
{
    glfwSetWindowSize((GLFWwindow*)os_window_, width_, height);
    height_ = height;
    return GLFW_kSuccess;
};

GLFW_TSuccess GLFW_Window::set_client_size(uint32_t width,
                                           uint32_t height) const
{
    glfwSetWindowSize((GLFWwindow*)os_window_, width, height);
    width_  = width;
    height_ = height;
    return GLFW_kSuccess;
};

void GLFW_Window::set_hint() const
{
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
}

uint16_t GLFW_Window::get_dpi_hint() const
{
    GLFWmonitor* display_index = glfwGetWindowMonitor((GLFWwindow*)os_window_);
    if (display_index == nullptr)
        return 0;

    const GLFWvidmode* mode = glfwGetVideoMode(display_index);
    if (mode == nullptr)
        return 0;

    return mode->width / width_;
};

} // namespace vektor