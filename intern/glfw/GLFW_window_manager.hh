#pragma once

#include <vector>

#include "GLFW_ISystem.hh"
#include "GLFW_IWindow.hh"

namespace vektor
{
class GLFW_WindowManager
{
public:
    GLFW_WindowManager();

    ~GLFW_WindowManager();

    GLFW_TSuccess                     add_window(GLFW_IWindow* glfw_window);

    GLFW_TSuccess                     remove_window(const GLFW_IWindow* glfw_window);

    GLFW_TSuccess                     get_active_window(GLFW_IWindow* glfw_window);

    GLFW_TSuccess                     set_active_window(GLFW_IWindow* glfw_window);

    void                              set_window_inactive(const GLFW_IWindow* glfw_window);

    GLFW_IWindow*                     get_active_window() const;

    const std::vector<GLFW_IWindow*>& get_windows() const;

    bool                              get_window_found(const GLFW_IWindow* window) const;

    GLFW_IWindow*                     get_window_associated_with_os_window(const void* osWindow);

protected:
    std::vector<GLFW_IWindow*> windows_;
    GLFW_IWindow*              active_window_;
};
} // namespace vektor