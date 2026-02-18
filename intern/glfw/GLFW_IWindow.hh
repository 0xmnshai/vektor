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
    virtual ~GLFW_IWindow()                                                    = default;

    virtual void              init() const                                     = 0;

    virtual bool              get_valid() const                                = 0;

    virtual void              set_title(const char* title)                     = 0;

    virtual std::string       get_title() const                                = 0;

    virtual void              set_path(const char* filepath)                   = 0;

    virtual void              get_window_bounds(GLFW_Rect& bounds) const       = 0;

    virtual void              get_client_bounds(GLFW_Rect& bounds) const       = 0;

    virtual GLFW_TUserDataPtr get_user_data() const                            = 0;

    virtual GLFW_TWindowState get_window_state() const                         = 0;

    virtual GLFW_TSuccess     invalidate() const                               = 0;

    virtual void              set_user_data(GLFW_TUserDataPtr user_data) const = 0;

    virtual GLFW_TSuccess     set_client_width(uint32_t width) const           = 0;

    virtual void              set_modified_state(const bool modified) const    = 0;

    virtual GLFW_TSuccess     set_client_height(uint32_t height) const;

    virtual GLFW_TSuccess     set_client_size(uint32_t width,
                                              uint32_t height) const;

    virtual uint16_t          get_dpi_hint() const;

private:
    GLFW_TUserDataPtr user_data_;
};
} // namespace vektor