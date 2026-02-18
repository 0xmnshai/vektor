#include "GLFW_Event.hh"
#include <cstring>

namespace vektor
{

GLFW_Event::GLFW_Event(GLFW_TEventType type,
                       GLFW_IWindow*   window,
                       uint64_t        time,
                       const void*     data,
                       size_t          data_size)
    : type_(type)
    , window_(window)
    , time_(time)
{
    if (data && data_size > 0)
    {
        data_.resize(data_size);
        std::memcpy(data_.data(), data, data_size);
    }
}

GLFW_TEventType GLFW_Event::get_type() const
{
    return type_;
}

uint64_t GLFW_Event::get_time() const
{
    return time_;
}

GLFW_IWindow* GLFW_Event::get_window() const
{
    return window_;
}

GLFW_TEventDataPtr GLFW_Event::get_data() const
{
    return data_.empty() ? nullptr : data_.data();
}

} // namespace vektor
