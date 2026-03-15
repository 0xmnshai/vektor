#pragma once
#include <QWidget>

namespace qt::dock {
class AssetsBrowserWidget : public QWidget {
    Q_OBJECT
public:
    explicit AssetsBrowserWidget(QWidget *parent = nullptr);
};
} // namespace qt::dock
