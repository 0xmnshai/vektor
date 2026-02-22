#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <cstring>
#include <iostream> // IWYU pragma: keep

#include "GLFW_ISystem.hh"
#include "GLFW_IWindow.hh"
#include "GLFW_System.hh"
#include "GLFW_context.hh"
#include "GLFW_types.h"

namespace vektor
{

#define GLFW_WINDOW_STATE_DEFAULT GLFW_kWindowStateMaximized

static struct WMInitStruct
{

    int32_t           size;
    int32_t           start;

    GLFW_TWindowState windowstate   = GLFW_WINDOW_STATE_DEFAULT;

    bool              window_frame  = true;
    bool              window_focus  = true;
    bool              native_pixels = true;
} wm_init_state;

class GLFW_Window : public GLFW_IWindow
{
public:
    GLFW_Window();

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
                                           const GLFW_IWindow* parent_window = nullptr);

    virtual void             init() override;

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

    inline void              set_user_data(GLFW_TUserDataPtr user_data) override { user_data_ = user_data; }

    virtual uint64_t         get_milli_seconds() const { return glfwGetTime() * 1000; }

    inline void*             get_os_window() const { return os_window_; }

    inline void              set_path(const char* filepath) override { filepath_ = filepath; };

    virtual bool             set_window_cursor_visibility(bool visible) const;

    GLFW_TSuccess            set_cursor_visibility(bool visible) const override;

    bool                     has_cursor_shape(GLFW_TStandardCursor shape);

    static GLFWcursor*       get_standard_cursor_shape(GLFW_TStandardCursor shape);

    static void              set_cursor_shape(GLFW_TStandardCursor shape);

    virtual GLFW_TSuccess    set_window_custom_cursor_generator(GLFW_CursorGenerator* cursor_generator);

    virtual GLFW_TSuccess    set_custom_cursor_generator(GLFW_CursorGenerator* cursor_generator) override;

    virtual GLFW_TSuccess    set_window_custom_cursor_shape(const uint8_t* bitmap,
                                                            const uint8_t* mask,
                                                            const int      size[2],
                                                            const int      hot_size[2],
                                                            bool           can_invert_color);

    virtual GLFW_TSuccess    set_custom_cursor_shape(const uint8_t* bitmap,
                                                     const uint8_t* mask,
                                                     const int      size[2],
                                                     const int      hot_spot[2],
                                                     bool           can_invert_color) override;

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
    GLFW_Context*        context_;
    GLFW_TUserDataPtr    user_data_;
    void*                os_window_;
    static uint32_t      width_;
    static uint32_t      height_;
    GLFW_TWindowState    state_;
    const char*          title_;
    static bool          is_valid_;
    static const char*   filepath_;
    static bool          cursor_visible_;
    GLFW_TStandardCursor cursor_shape_;
    GLFWcursor*          custom_cursor_;
    GLFW_System*         system_;
};
} // namespace vektor