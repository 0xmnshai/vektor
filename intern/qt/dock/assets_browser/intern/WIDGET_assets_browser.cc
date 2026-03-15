#include <QVBoxLayout>
#include <QLabel>

#include "../WIDGET_assets_browser.h"

namespace qt::dock {
AssetsBrowserWidget::AssetsBrowserWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Assets Browser Placeholder", this));
    layout->addStretch();
}
} // namespace qt::dock
