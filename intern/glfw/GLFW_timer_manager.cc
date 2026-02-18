#include <cstdint>
#include "GLFW_types.h"

#include "GLFW_timer_manager.hh"

namespace vektor
{

GLFW_TimerManager::GLFW_TimerManager() = default;

GLFW_TimerManager::~GLFW_TimerManager()
{
    dispose_timers();
}

uint32_t GLFW_TimerManager::get_num_timers() const
{
    return static_cast<uint32_t>(timers_.size());
}

bool GLFW_TimerManager::get_timer_found(GLFW_TimerTask* timer)
{
    TTimerVector::const_iterator iter = std::find(timers_.begin(), timers_.end(), timer);
    return iter != timers_.end();
}

GLFW_TSuccess GLFW_TimerManager::add_timer(GLFW_TimerTask* timer)
{
    GLFW_TSuccess success;
    if (!get_timer_found(timer))
    {
        timers_.push_back(timer);
        success = GLFW_kSuccess;
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

GLFW_TSuccess GLFW_TimerManager::remove_timer(GLFW_TimerTask* timer)
{
    GLFW_TSuccess          success;
    TTimerVector::iterator iter = std::find(timers_.begin(), timers_.end(), timer);
    if (iter != timers_.end())
    {
        /* Remove the timer task. */
        timers_.erase(iter);
        delete timer;
        success = GLFW_kSuccess;
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

uint64_t GLFW_TimerManager::next_fire_time()
{
    uint64_t               smallest = GLFW_kFireTimeNever;
    TTimerVector::iterator iter;

    for (iter = timers_.begin(); iter != timers_.end(); ++iter)
    {
        const uint64_t next = (*iter)->get_next();
        smallest            = std::min(next, smallest);
    }

    return smallest;
}

bool GLFW_TimerManager::fire_timers(uint64_t time)
{
    TTimerVector::iterator iter;
    bool                   anyProcessed = false;

    for (iter = timers_.begin(); iter != timers_.end(); ++iter)
    {
        if (fire_timer(time, *iter))
        {
            anyProcessed = true;
        }
    }

    return anyProcessed;
}

bool GLFW_TimerManager::fire_timer(uint64_t        time,
                                   GLFW_TimerTask* task)
{
    uint64_t next = task->get_next();

    if (time > next)
    {
        GLFW_TimerProcPtr timer_proc = task->get_time_proc();
        uint64_t          start      = task->get_start();
        timer_proc(task, time - start);

        uint64_t interval = task->get_interval();
        uint64_t numCalls = (next - start) / interval;
        numCalls++;
        next = start + numCalls * interval;
        task->set_next(next);

        return true;
    }
    return false;
}

void GLFW_TimerManager::dispose_timers()
{
    while (timers_.empty() == false)
    {
        delete timers_[0];
        timers_.erase(timers_.begin());
    }
}

} // namespace vektor