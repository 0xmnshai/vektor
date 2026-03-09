#include "VPI_Window.hh"

namespace vpi {

void VPI_Window::create_window(char const *title,
                               int32_t left,
                               int32_t top,
                               uint32_t width,
                               uint32_t height,
                               VPI_IWindow const *parent_window) noexcept
{
}

VPI_TSuccess VPI_Window::dispose() noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::get_title(char const **title) const noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::set_title(char const *title) noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::get_position(int32_t *left, int32_t *top) const noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::set_position(int32_t left, int32_t top) noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::get_size(uint32_t *width, uint32_t *height) const noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::set_size(uint32_t width, uint32_t height) noexcept
{
  return VPI_kSuccess;
}

VPI_TWindowState VPI_Window::get_state() const noexcept
{
  return VPI_kWindowStateNormal;
}

VPI_TSuccess VPI_Window::set_state(VPI_TWindowState state) noexcept
{
  return VPI_kSuccess;
}

bool VPI_Window::process_events(bool wait_for_event) noexcept
{
  return false;
}

VPI_TSuccess VPI_Window::get_native_handle(void const **handle) const noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::add_event_consumer(VPI_IEventConsumer *consumer) noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::get_cursor_position(int32_t *x, int32_t *y) const noexcept
{
  return VPI_kSuccess;
}

VPI_TSuccess VPI_Window::set_cursor_position(int32_t x, int32_t y) noexcept
{
  return VPI_kSuccess;
}

void VPI_Window::get_window_bounds(VPI_Rect &bounds) const {}

void VPI_Window::get_client_bounds(VPI_Rect &bounds) const {}

}  // namespace vpi
