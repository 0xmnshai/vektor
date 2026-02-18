#pragma once

#include <cstdint>

#include "GLFW_ITimer_Task.hh"
#include "GLFW_types.h"

namespace vektor
{
class GLFW_TimerTask : public GLFW_ITimerTask
{
public:
    GLFW_TimerTask(uint64_t          start,
                   uint64_t          interval,
                   GLFW_TimerProcPtr timer_proc,
                   GLFW_TUserDataPtr user_data = nullptr)
        : start_(start)
        , interval_(interval)
        , next_(start)
    {
    }

    uint64_t          get_start() const { return start_; }

    uint64_t          get_interval() const { return interval_; }

    uint64_t          get_next() const { return next_; }

    void              set_next(uint64_t next) { next_ = next; }

    GLFW_TimerProcPtr get_time_proc() const override { return timer_proc_; }

    void              set_time_proc(const GLFW_TimerProcPtr timer_proc) const override { timer_proc_ = timer_proc; }

    GLFW_TUserDataPtr get_user_data() const override { return user_data_; }

    void              set_user_data(const GLFW_TUserDataPtr user_data) const override { user_data_ = user_data; }

private:
    uint64_t                 start_;
    uint64_t                 interval_;
    uint64_t                 next_;
    static GLFW_TimerProcPtr timer_proc_;
    static GLFW_TUserDataPtr user_data_;
};
} // namespace vektor
