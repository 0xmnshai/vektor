#include "GLFW_window_manager.hh"
#include "GLFW_ISystem.hh"
#include "GLFW_IWindow.hh"
#include "GLFW_window.hh"

namespace vektor
{
GLFW_WindowManager::GLFW_WindowManager()
    : active_window_(nullptr)
{
}

GLFW_WindowManager::~GLFW_WindowManager() = default;

GLFW_TSuccess GLFW_WindowManager::add_window(GLFW_IWindow* window)
{
    GLFW_TSuccess success = GLFW_kFailure;
    if (window)
    {
        if (!get_window_found(window))
        {
            windows_.push_back(window);
            success = GLFW_kSuccess;
        }
    }
    return success;
}

GLFW_TSuccess GLFW_WindowManager::remove_window(const GLFW_IWindow* window)
{
    GLFW_TSuccess success = GLFW_kFailure;
    if (window)
    {
        std::vector<GLFW_IWindow*>::iterator result = find(windows_.begin(), windows_.end(), window);
        if (result != windows_.end())
        {
            set_window_inactive(window);
            windows_.erase(result);
            success = GLFW_kSuccess;
        }
    }
    return success;
}

bool GLFW_WindowManager::get_window_found(const GLFW_IWindow* window) const
{
    bool found = false;
    if (window)
    {
        std::vector<GLFW_IWindow*>::const_iterator result = find(windows_.begin(), windows_.end(), window);
        if (result != windows_.end())
        {
            found = true;
        }
    }
    return found;
}

GLFW_TSuccess GLFW_WindowManager::set_active_window(GLFW_IWindow* window)
{
    GLFW_TSuccess success = GLFW_kSuccess;
    if (window != active_window_)
    {
        if (get_window_found(window))
        {
            active_window_ = window;
        }
        else
        {
            success = GLFW_kFailure;
        }
    }
    return success;
}

GLFW_IWindow* GLFW_WindowManager::get_active_window() const
{
    return active_window_;
}

void GLFW_WindowManager::set_window_inactive(const GLFW_IWindow* window)
{
    if (window == active_window_)
    {
        active_window_ = nullptr;
    }
}

const std::vector<GLFW_IWindow*>& GLFW_WindowManager::get_windows() const
{
    return windows_;
}

GLFW_IWindow* GLFW_WindowManager::get_window_associated_with_os_window(const void* osWindow)
{
    std::vector<GLFW_IWindow*>::iterator iter;

    for (iter = windows_.begin(); iter != windows_.end(); ++iter)
    {
        GLFW_Window* window = static_cast<GLFW_Window*>(*iter);
        if (window->get_os_window() == osWindow)
        {
            return *iter;
        }
    }
    return nullptr;
}

} // namespace vektor