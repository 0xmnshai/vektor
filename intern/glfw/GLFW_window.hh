#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <cstring>
#include <iostream> // IWYU pragma: keep

#include "GLFW_ISystem.hh"
#include "GLFW_IWindow.hh"
#include "GLFW_context.hh"
#include "GLFW_types.h"

namespace vektor
{
class GLFW_Window : public GLFW_IWindow
{
public:
    GLFW_Window(uint32_t                  width,
                uint32_t                  height,
                GLFW_TWindowState         state,
                const GLFW_ContextParams& context_params,
                const bool                exclusive = false);

    ~GLFW_Window();

    virtual GLFW_IWindow*    create_window(const char*         title,
                                           int32_t             left,
                                           int32_t             top,
                                           uint32_t            width,
                                           uint32_t            height,
                                           GLFW_TWindowState   state,
                                           GLFW_GPUSettings    gpu_settings,
                                           const bool          exclusive     = false,
                                           const bool          is_dialog     = false,
                                           const GLFW_IWindow* parent_window = nullptr) = 0;

    virtual void             init() const override;

    virtual void             set_title(const char* title) override;

    virtual void             main_loop();

    virtual GLFW_TSuccess    set_client_width(uint32_t width) const override;

    virtual GLFW_TSuccess    set_client_height(uint32_t height) const override;

    virtual GLFW_TSuccess    set_client_size(uint32_t width,
                                             uint32_t height) const override;

    virtual void             get_window_bounds(GLFW_Rect& bounds) const override;

    virtual void             get_client_bounds(GLFW_Rect& bounds) const override;

    virtual uint16_t         get_dpi_hint() const override;

    GLFW_TSuccess            set_window_state(GLFW_TWindowState state);

    GLFW_TWindowState        get_window_state() const override;

    GLFW_TSuccess            invalidate() const override;

    void                     set_hint() const;

    virtual void             set_modified_state(const bool modified) const override;

    inline GLFW_Context*     get_context() const { return context_; }

    inline bool              get_valid() const override { return context_ != nullptr; }

    virtual std::string      get_title() const override { return title_; };

    inline GLFW_TUserDataPtr get_user_data() const override { return user_data_; }

    inline void              set_user_data(GLFW_TUserDataPtr user_data) { user_data_ = user_data; }

    virtual uint64_t         get_milli_seconds() const { return glfwGetTime() * 1000; }

    inline void*             get_os_window() const { return os_window_; }

    inline void                     set_file_path(const char* filepath) const
    {   
        filepath_ = filepath;
    };


protected:
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

private:
    GLFW_Context*     context_;
    GLFW_TUserDataPtr user_data_;
    void*             os_window_;
    static uint32_t   width_;
    static uint32_t   height_;
    GLFW_TWindowState state_;
    const char*       title_;
    static bool       is_valid_;
    static const char* filepath_;
};
} // namespace vektor