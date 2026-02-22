#pragma once

#include "GLFW_ISystem.hh" 

namespace vektor
{
class GLFW_Context
{
protected:
    static thread_local inline GLFW_Context* active_context_;

public:
    GLFW_Context(const GLFW_ContextParams& context_params)
        : context_params_(context_params)
    {
    }

    ~GLFW_Context()
    {
        if (active_context_ == this)
        {
            active_context_ = nullptr;
        }
    };

    static inline GLFW_Context* get_active_drawing_context() { return active_context_; }

    virtual GLFW_TSuccess       update_drawing_context() { return GLFW_kFailure; }

    void*                       get_user_data() { return user_data_; }

    void                        set_user_data(void* user_data) { user_data_ = user_data; }

private:
    GLFW_ContextParams context_params_;
    void*              user_data_ = nullptr;
};
} // namespace vektor
