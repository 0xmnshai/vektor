#include "VPI_QtEventConsumer.hh"
#include "CLG_log.h"
#include "intern/VPI_Event.hh"

namespace vpi {

CLG_LOGREF_DECLARE_GLOBAL(QT_EventConsumer, "qt.event.consumer");

VPI_TSuccess VPI_QtEventConsumer::consume_event(VPI_Event *event) const noexcept
{
  CLOG_INFO(QT_EventConsumer, "Consuming event of type: %d", event->get_type());

  if (event->get_type() == VPI_kKey) {
    auto *ke = dynamic_cast<vpi::VPI_KeyEvent *>(event);
    if (ke) {
      CLOG_INFO(QT_EventConsumer, "Consuming event key: %d", ke->data.key);
    }
  }
  else if (event->get_type() == VPI_kMouseButton) {
    auto *me = dynamic_cast<vpi::VPI_MouseEvent *>(event);
    if (me) {
      CLOG_INFO(QT_EventConsumer,
                "Consuming Button: %d at ( %d , %d )",
                me->data.button,
                me->data.x,
                me->data.y);
    }
  }

  (void)event->consume();
  return VPI_kSuccess;
};
}  // namespace vpi
