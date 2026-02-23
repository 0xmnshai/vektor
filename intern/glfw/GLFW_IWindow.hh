#pragma once

#include <string>
#include "GLFW_ISystem.hh"
#include "GLFW_rect.hh"
#include "GLFW_types.h"

namespace vektor
{
class GLFW_IWindow
{
public:
    virtual ~GLFW_IWindow()                                                                       = default;

    virtual void              init()                                                              = 0;

    virtual bool              get_valid() const                                                   = 0;

    virtual void              set_title(const char* title)                                        = 0;

    virtual std::string       get_title() const                                                   = 0;

    virtual void              set_path(const char* filepath)                                      = 0;

    virtual void              get_window_bounds(GLFW_Rect& bounds) const                          = 0;

    virtual void              get_client_bounds(GLFW_Rect& bounds) const                          = 0;

    virtual GLFW_TUserDataPtr get_user_data() const                                               = 0;

    virtual GLFW_TWindowState get_window_state() const                                            = 0;

    virtual GLFW_TSuccess     invalidate() const                                                  = 0;

    virtual void              set_user_data(GLFW_TUserDataPtr user_data)                          = 0;

    virtual GLFW_TSuccess     set_client_width(uint32_t width) const                              = 0;

    virtual void              set_modified_state(const bool modified) const                       = 0;

    virtual GLFW_TSuccess     set_client_height(uint32_t height) const                            = 0;

    virtual GLFW_TSuccess     set_client_size(uint32_t width,
                                              uint32_t height) const                              = 0;

    virtual uint16_t          get_dpi_hint() const                                                = 0;

    virtual GLFW_TSuccess     set_cursor_visibility(bool visible) const                           = 0;

    virtual GLFW_TSuccess     set_custom_cursor_generator(GLFW_CursorGenerator* cursor_generator) = 0;

    virtual GLFW_TSuccess     set_custom_cursor_shape(const uint8_t* bitmap,
                                                      const uint8_t* mask,
                                                      const int      size[2],
                                                      const int      hot_spot[2],
                                                      bool           can_invert_color)                      = 0;

private:
    GLFW_TUserDataPtr user_data_;

    MEM_CXX_CLASS_ALLOC_FUNCS("GLFW:GLFW_IWindow")
};
} // namespace vektor