#pragma once

#include <cstddef>
#include "GLFW_types.h"
#include "MEM_gaurdalloc.hh"

namespace vektor
{
class GLFW_IWindow;

class GLFW_IEvent
{
public:
    virtual ~GLFW_IEvent()                        = default;

    virtual GLFW_TEventType    get_type() const   = 0;

    virtual uint64_t           get_time() const   = 0;

    virtual GLFW_IWindow*      get_window() const = 0;

    virtual GLFW_TEventDataPtr get_data() const   = 0;

    MEM_CXX_CLASS_ALLOC_FUNCS("GLFW:GLFW_IEvent")
};
} // namespace vektor