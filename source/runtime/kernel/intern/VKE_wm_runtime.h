#pragma once

#include "../../runtime/dna/DNA_listBase.h"
#include "../../runtime/windowmanager/wm_event_system.h"

#include "../vklib/VKE_list_base.h"
#include "VK_report.hh"

namespace vektor
{
struct wmKeyConfig;

struct WindowRuntime
{
    WindowRuntime() = default;
    ~WindowRuntime();

    ListBaseT<wmEvent>        event_queue        = {nullptr, nullptr};

    // Screen handlesrs
    ListBaseT<wmEventHandler> handlers           = {nullptr, nullptr};

    wmEvent*                  event_last_handled = nullptr;

    wmEvent*                  eventstate         = nullptr;

    void*                     glfw_win           = nullptr;

    ListBaseT<wmKeyConfig>    keyconfigs         = {nullptr, nullptr};

    wmKeyConfig*              addonconf          = nullptr;

    wmKeyConfig*              defaultconf        = nullptr;

    wmKeyConfig*              userconf           = nullptr;

    wmWindow*                 winactive          = nullptr;

    wmWindow*                 windrawable        = nullptr;

    void*                     gpuctx             = nullptr;

    ListBaseT<wmTimer>        timers             = {nullptr, nullptr};

    ReportList                reports;
};
} // namespace vektor