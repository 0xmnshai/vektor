#include "GLFW_IEventConsumer.hh"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "GLFW_Event.hh"

namespace vektor
{
bool GLFW_IEventConsumer::process_event(const GLFW_IEvent* event)
{
    if (!event)
        return false;

    auto*         raw_window = event->get_window();
    GLFW_IWindow* window     = reinterpret_cast<GLFW_IWindow*>(raw_window);
    const void*   data       = event->get_data();

    switch (event->get_type())
    {
        case GLFW_kEventWindowUpdate:
        {
            if (window)
            {
                // Clear the buffer
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            break;
        }
        case GLFW_kEventWindowSize:
        {
            if (data)
            {
                const auto* size_data = static_cast<const GLFW_EventWindowSizeData*>(data);
                glViewport(0, 0, size_data->width, size_data->height);
            }
            break;
        }
        case GLFW_kEventWindowMove:
        {
            break;
        }
        case GLFW_kEventWindowActivate:
        case GLFW_kEventWindowDeactivate:
        {
            break;
        }
        case GLFW_kEventWindowClose:
        case GLFW_kEventQuitRequest:
        {
            break;
        }
        case GLFW_kEventCursorMove:
        {
            if (data && window)
            {
                const auto* cursor_data = static_cast<const GLFW_EventCursorData*>(data);
                // Mouse position: cursor_data->x, cursor_data->y
                (void)cursor_data;
            }
            break;
        }
        case GLFW_kEventButtonDown:
        case GLFW_kEventButtonUp:
        {
            if (data)
            {
                const auto* btn_data = static_cast<const GLFW_EventButtonData*>(data);
                // Button state: btn_data->button, btn_data->action, btn_data->mods
                (void)btn_data;
            }
            break;
        }
        case GLFW_kEventWheel:
        {
            if (data)
            {
                const auto* scroll_data = static_cast<const GLFW_EventScrollData*>(data);
                // Scroll offset: scroll_data->xoffset, scroll_data->yoffset
                (void)scroll_data;
            }
            break;
        }
        case GLFW_kEventKeyDown:
        case GLFW_kEventKeyUp:
        {
            if (data)
            {
                const auto* key_data = static_cast<const GLFW_EventKeyData*>(data);
                // Key state: key_data->key, key_data->scancode, key_data->action, key_data->mods
                (void)key_data;
            }
            break;
        }
        default:
            break;
    }

    return true;
}
} // namespace vektor
