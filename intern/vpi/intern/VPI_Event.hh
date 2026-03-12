#pragma once

#include "VPI_IEvent.h"
#include "VPI_Types.h"

namespace vpi {

class VPI_IWindow;

class VPI_Event : public VPI_IEvent {
 public:
  explicit VPI_Event(VPI_EventType type, VPI_IWindow *window) : type_(type), window_(window) {}
  virtual ~VPI_Event() = default;

  [[nodiscard]] inline VPI_EventType get_type() const noexcept override
  {
    return type_;
  };

  [[nodiscard]] VPI_TSuccess is_consumed() const noexcept override
  {
    return consumed_ ? VPI_kSuccess : VPI_kFailure;
  }

  [[nodiscard]] VPI_TSuccess consume() const noexcept override
  {
    consumed_ = true;
    return VPI_kSuccess;
  }

  [[nodiscard]] VPI_IWindow *get_window() const noexcept
  {
    return window_;
  }

 protected:
  VPI_EventType type_;
  VPI_IWindow *window_;
  mutable bool consumed_ = false;
};

class VPI_KeyEvent : public VPI_Event {
 public:
  VPI_KeyEvent(VPI_IWindow *window, VPI_TEventKeyData data)
      : VPI_Event(VPI_kKey, window), data(data)
  {
  }
  VPI_TEventKeyData data;
};

class VPI_MouseEvent : public VPI_Event {
 public:
  VPI_MouseEvent(VPI_IWindow *window, VPI_TEventMouseButtonData data)
      : VPI_Event(VPI_kMouseButton, window), data(data)
  {
  }
  VPI_TEventMouseButtonData data;
};

class VPI_MouseMoveEvent : public VPI_Event {
 public:
  VPI_MouseMoveEvent(VPI_IWindow *window, VPI_TEventCursorData data)
      : VPI_Event(VPI_kCursorMove, window), data(data)
  {
  }
  VPI_TEventCursorData data;
};

class VPI_MouseWheelEvent : public VPI_Event {
 public:
  VPI_MouseWheelEvent(VPI_IWindow *window, VPI_TEventMouseWheelData data)
      : VPI_Event(VPI_kMouseWheel, window), data(data)
  {
  }
  VPI_TEventMouseWheelData data;
};

class VPI_WindowEvent : public VPI_Event {
 public:
  VPI_WindowEvent(VPI_EventType type, VPI_IWindow *window) : VPI_Event(type, window) {}
};

}  // namespace vpi
