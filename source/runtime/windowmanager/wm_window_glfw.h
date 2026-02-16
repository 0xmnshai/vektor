#pragma once

#include <memory>

#include "../../intern/glfw/GLFW_rect.hh"
#include "wm_types.h"

namespace vektor
{

struct GLFWwindow;

class GLFW_Window
{
public:
    static void         init(int    argc,
                             char** argv);

    static void         create_window(std::shared_ptr<wmWindow> window);

    static void         main_loop();

    virtual uint64_t    get_milli_seconds() const                  = 0;

    virtual void        set_title(const char* title)               = 0;

    virtual std::string get_title() const                          = 0;

    virtual void        set_file_path(const char* filepath)        = 0;

    virtual void        get_window_bounds(GLFW_Rect& bounds) const = 0;

    virtual void        set_modified_state(bool modified)          = 0;

private:
    static void error_callback(int         error,
                               const char* description);

    static void key_callback(GLFWwindow* window,
                             int         key,
                             int         scancode,
                             int         action,
                             int         mods);
    static void mouse_button_callback(GLFWwindow* window,
                                      int         button,
                                      int         action,
                                      int         mods);
    static void cursor_pos_callback(GLFWwindow* window,
                                    double      xpos,
                                    double      ypos);
    static void scroll_callback(GLFWwindow* window,
                                double      xoffset,
                                double      yoffset);
    static void frame_buffer_size_callback(GLFWwindow* window,
                                           int         width,
                                           int         height);
};
} // namespace vektor