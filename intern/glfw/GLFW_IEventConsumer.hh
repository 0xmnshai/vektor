
#pragma once

#include "GLFW_IEvent.hh"

namespace vektor
{
class GLFW_IEventConsumer
{
public:
    virtual ~GLFW_IEventConsumer()                       = default;

    virtual bool process_event(const GLFW_IEvent* event) = 0;

    MEM_CXX_CLASS_ALLOC_FUNCS("GLFW:GLFW_IEventConsumer")
};
} // namespace vektor