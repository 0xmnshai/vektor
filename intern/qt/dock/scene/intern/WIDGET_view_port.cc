#include "../WIDGET_view_port.h"
#include <QSizePolicy>

namespace qt::dock {
void ViewportWidget::init()
{
  vpi::VPI_GLWidget::init();
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMinimumSize(400, 300);
  // we will do rest of the things here if needed
}
}  // namespace qt::dock
