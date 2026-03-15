#include <QLabel>
#include <QVBoxLayout>

#include "../WIDGET_properties.h"

namespace qt::dock {
PropertiesWidget::PropertiesWidget(QWidget *parent) : QWidget(parent)
{
  auto *layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Properties Placeholder", this));
  layout->addStretch();
}
}  // namespace qt::dock
