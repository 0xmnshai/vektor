
#pragma once

#include "GLFW_types.h"

namespace vektor
{
class GLFW_ITimerTask
{
public:
    virtual ~GLFW_ITimerTask()                                                         = default;

    virtual GLFW_TimerProcPtr get_time_proc() const                                    = 0;

    virtual void              set_time_proc(const GLFW_TimerProcPtr timer_proc) const = 0;

    virtual GLFW_TUserDataPtr get_user_data() const                                    = 0;

    virtual void              set_user_data(const GLFW_TUserDataPtr user_data) const   = 0;

    //   MEM_CXX_CLASS_ALLOC_FUNCS("GLFW_:GLFW_ITimerTask")
};
} // namespace vektor