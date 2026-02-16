
#pragma once

#include "../gaurdalloc/MEM_gaurdalloc.hh"
#include "VKI_types.h"

namespace vektor
{
class VKI_ITimerTask
{

public:
    virtual ~VKI_ITimerTask()                                                = default;

    virtual VKI_TimerProcPtr getTimerProc() const                            = 0;

    virtual void             setTimerProc(const VKI_TimerProcPtr timer_proc) = 0;

    virtual VKI_TUserDataPtr getUserData() const                             = 0;

    virtual void             setUserData(const VKI_TUserDataPtr user_data)   = 0;

    MEM_CXX_CLASS_ALLOC_FUNCS("VKI:VKI_ITimerTask")
};
} // namespace vektor