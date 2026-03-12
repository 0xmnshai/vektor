#pragma once

#include <QElapsedTimer>
#include <new>

#include "VPI_ISystem.h"
#include "VPI_Types.h"
#include "intern/VPI_QtWindow.hh"

#include "../../intern/gaurdalloc/MEM_gaurdalloc.h"

namespace vpi {
class VPI_System : public QObject, public VPI_ISystem {
  Q_OBJECT
 public:
  explicit VPI_System();
  ~VPI_System() override;

  void *operator new(size_t size)
  {
    return MEM_mallocN(size, "VPI_System");
  }

  void *operator new(size_t size, std::align_val_t alignment)
  {
    return mem_guarded::internal::mem_mallocN_aligned_ex(
        size,
        static_cast<size_t>(alignment),
        "VPI_System",
        mem_guarded::internal::DestructorType::Trivial);
  }

  void *operator new(size_t /*size*/, void *ptr) noexcept
  {
    return ptr;
  }

  void operator delete(void *ptr)
  {
    MEM_freeN(ptr);
  }

  void operator delete(void *ptr, std::align_val_t /*alignment*/)
  {
    MEM_freeN(ptr);
  }

  void operator delete(void *ptr, void * /*place*/) noexcept
  {
    MEM_freeN(ptr);
  }

  [[nodiscard]] VPI_Window *create_window(char const *title,
                                          int32_t left,
                                          int32_t top,
                                          uint32_t width,
                                          uint32_t height,
                                          VPI_Window const *parent_window) noexcept override;

  VPI_TSuccess init() override;

  VPI_TSuccess exit(bool &is_running) override;

  [[nodiscard]] uint64_t get_milliseconds() const noexcept override;

  [[nodiscard]] VPI_Window *get_window_under_cursor(int32_t x, int32_t y) const noexcept override;

  [[nodiscard]] VPI_TSuccess register_window(VPI_Window *window) noexcept;

 protected:
  VPI_TSuccess process_events_impl(bool wait_for_event) override;
  bool event_filter(QObject *obj, QEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override
  {
    return event_filter(obj, event);
  }

 private:
  VPI_QtWindow *qt_window_;
  QApplication *qt_app_;
  QElapsedTimer timer_;

  uint64_t start_time_;
};

}  // namespace vpi
