#include <QVBoxLayout>
#include <QLabel>

#include "../WIDGET_console.h"

namespace qt::dock {
ConsoleWidget::ConsoleWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Console Placeholder", this));
    layout->addStretch();
}
} // namespace qt::dock
