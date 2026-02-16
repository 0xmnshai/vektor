#pragma once

#include "../../runtime/dna/DNA_listBase.h"
#include "../../runtime/windowmanager/wm_event.h"
#include "../../runtime/windowmanager/wm_event_system.h"

namespace vektor
{
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
};
} // namespace vektor