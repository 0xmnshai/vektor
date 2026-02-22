#include "GLFW/glfw3.h"

#include "GLFW_ISystem.hh"
#include "GLFW_ITimer_Task.hh"
#include "GLFW_System.hh"
#include "GLFW_TimerTask.hh"

#include "../../intern/clog/COG_log.hh"

namespace vektor
{

CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_App,
                          "GLFW System");

void error_callback(int         error,
                    const char* description)
{
    CLOG_ERROR(CLG_LogRef_App, "GLFW Error %d: %s", error, description);
}

void BLI_system_backtrace_with_os_info(FILE*       fp,
                                       const void* os_info);

void BLI_system_backtrace_with_os_info(FILE* fp,
                                       const void* /*os_info*/)
{

#define SIZE 100
    void*  buffer[SIZE];
    int    nptrs;
    char** strings;
    int    i;

    // nptrs   = backtrace(buffer, SIZE);
    // strings = backtrace_symbols(buffer, nptrs);

    // for (i = 0; i < nptrs; i++)
    // {
    //     fputs(strings[i], fp);
    //     fputc('\n', fp);
    // }

    // free(strings);
};

void BLI_system_backtrace(FILE* fp);

void BLI_system_backtrace(FILE* fp)
{
    static std::mutex mutex;
    std::scoped_lock  lock(mutex);
    BLI_system_backtrace_with_os_info(fp, nullptr);
}

GLFW_TSuccess GLFW_System::init()
{
    event_manager_  = new GLFW_EventManager();
    timer_manager_  = new GLFW_TimerManager();
    window_manager_ = new GLFW_WindowManager();

    // will init system cc here which will init glfw window cc

    if (timer_manager_ && window_manager_ && event_manager_)
    {
        if (!glfwInit())
        {
            CLOG_ERROR(CLG_LogRef_App, "Failed to initialize GLFW");
        }

        glfwSetErrorCallback(error_callback);
        set_backtrace_fn(BLI_system_backtrace);

        return GLFW_kSuccess;
    }
    return GLFW_kFailure;
}

void GLFW_System::set_use_window_frame(bool use_window_frame) const
{
    use_window_frame_ = use_window_frame;
};

void          GLFW_System::get_window_bounds(GLFW_Rect& rect) const {}

GLFW_TSuccess GLFW_System::create_system(bool verbose,
                                         bool background)
{

    try
    {
        CLOG_INFO(CLG_LogRef_App, "Create GLFW system");
        system_backend_id_ = "GLFW";
        init();
    }
    catch (const std::runtime_error& e)
    {
        CLOG_STR_INFO_NOCHECK(CLG_LogRef_App, e.what());
        delete this;
    }

    return GLFW_kFailure;
};

GLFW_TSuccess GLFW_System::create_system_background()
{
    GLFW_TSuccess success;
    if (!system_)
    {

        CLOG_INFO(CLG_LogRef_App, "Create background system");
        success = create_system(false, true);
        if (success)
        {
            return success;
        }
    }
    else
    {
        success = GLFW_kFailure;
    }
    if (success)
    {
        success = init();
    }
    return success;
};

GLFW_TSuccess GLFW_System::exit()
{
    delete event_manager_;
    event_manager_ = nullptr;
    delete timer_manager_;
    timer_manager_ = nullptr;
    delete window_manager_;
    window_manager_ = nullptr;

    return GLFW_kSuccess;
}

GLFW_ITimerTask* GLFW_System::install_timer(uint64_t          delay,
                                            uint64_t          interval,
                                            GLFW_TimerProcPtr timer_proc,
                                            GLFW_TUserDataPtr user_data)
{
    uint64_t        millis = get_milli_seconds();
    GLFW_TimerTask* timer  = new GLFW_TimerTask(millis + delay, interval, timer_proc, user_data);

    if (timer)
    {
        if (timer_manager_->add_timer(timer) == GLFW_kSuccess)
        {
            timer_manager_->fire_timer(millis, timer);
        }
        else
        {
            delete timer;
            timer = nullptr;
        }
    }
    return timer;
}

GLFW_TSuccess GLFW_System::remove_timer(GLFW_ITimerTask* timerTask)
{
    GLFW_TSuccess success = GLFW_kFailure;
    if (timerTask)
    {
        success = timer_manager_->remove_timer((GLFW_TimerTask*)timerTask);
    }
    return success;
}

GLFW_TSuccess GLFW_System::dispose_window(GLFW_IWindow* window)
{
    GLFW_TSuccess success;

    if (window_manager_->get_window_found(window))
    {
        event_manager_->remove_window_events(window);
        success = window_manager_->remove_window(window);
        if (success)
        {
            delete window;
        }
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
};

bool GLFW_System::valid_window(GLFW_IWindow* window) const
{
    return window_manager_->get_window_found(window);
}

void GLFW_System::dispatch_events()
{
    if (event_manager_)
    {
        event_manager_->dispatch_events();
    }

    timer_manager_->fire_timers(get_milli_seconds());
};

uint64_t GLFW_System::get_milli_seconds() const
{
    return glfwGetTime() * 1000;
    // return (uint64_t)([[NSProcessInfo processInfo] systemUptime] * 1000);
};

GLFW_TSuccess GLFW_System::add_event_consumer(GLFW_IEventConsumer* consumer)
{
    GLFW_TSuccess success;
    if (event_manager_)
    {
        success = event_manager_->add_consumer(consumer);
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

GLFW_TSuccess GLFW_System::remove_event_consumer(GLFW_IEventConsumer* consumer)
{
    GLFW_TSuccess success;
    if (event_manager_)
    {
        success = event_manager_->remove_consumer(consumer);
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

GLFW_TSuccess GLFW_System::push_event(std::unique_ptr<const GLFW_IEvent> event)
{
    GLFW_TSuccess success;
    if (event_manager_)
    {
        success = event_manager_->push_event(std::move(event));
    }
    else
    {
        success = GLFW_kFailure;
    }
    return success;
}

} // namespace vektor
