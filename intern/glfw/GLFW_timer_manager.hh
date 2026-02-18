#pragma once

#include <cstdint>
#include <vector>

#include "GLFW_ISystem.hh"
#include "GLFW_TimerTask.hh"

namespace vektor
{
class GLFW_TimerManager
{
public:
    GLFW_TimerManager();
    ~GLFW_TimerManager();

    void          dispose_timers();

    uint32_t      get_num_timers() const;

    bool          get_timer_found(GLFW_TimerTask* timer);

    GLFW_TSuccess add_timer(GLFW_TimerTask* timer);

    GLFW_TSuccess remove_timer(GLFW_TimerTask* timer);

    uint64_t      next_fire_time();

    bool          fire_timers(uint64_t time);

    bool          fire_timer(uint64_t        time,
                             GLFW_TimerTask* task);

protected:
    using TTimerVector = std::vector<GLFW_TimerTask*>;
    TTimerVector timers_;

    // MEM_CXX_CLASS_ALLOC_FUNCS
};
} // namespace vektor