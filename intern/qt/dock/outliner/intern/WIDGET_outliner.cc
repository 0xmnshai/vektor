#include <QLabel>
#include <QVBoxLayout>

#include "../WIDGET_outliner.h"

namespace qt::dock {
OutlinerWidget::OutlinerWidget(QWidget *parent) : QWidget(parent)
{
  auto *layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Outliner Placeholder", this));
  layout->addStretch();
}
}  // namespace qt::dock
