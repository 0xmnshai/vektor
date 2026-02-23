#pragma once
#include <vector>
#include "GLFW_IEvent.hh"
#include "MEM_gaurdalloc.hh"

namespace vektor
{
struct GLFW_EventKeyData
{
    int key;
    int scancode;
    int action;
    int mods;
};

struct GLFW_EventButtonData
{
    int button;
    int action;
    int mods;
};

struct GLFW_EventCursorData
{
    double x;
    double y;
};

struct GLFW_EventScrollData
{
    double xoffset;
    double yoffset;
};

struct GLFW_EventWindowSizeData
{
    int width;
    int height;
};

class GLFW_Event : public GLFW_IEvent
{
public:
    GLFW_Event(GLFW_TEventType type,
               GLFW_IWindow*   window,
               uint64_t        time,
               const void*     data,
               size_t          data_size);

    ~GLFW_Event() = default;

    GLFW_TEventType    get_type() const override;
    uint64_t           get_time() const override;
    GLFW_IWindow*      get_window() const override;
    GLFW_TEventDataPtr get_data() const override;

private:
    GLFW_TEventType      type_;
    GLFW_IWindow*        window_;
    uint64_t             time_;
    std::vector<uint8_t> data_;

    MEM_CXX_CLASS_ALLOC_FUNCS("GLFW:GLFW_Event")
};
} // namespace vektor