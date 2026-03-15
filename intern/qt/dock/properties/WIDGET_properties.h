#pragma once
#include <QWidget>

namespace qt::dock {
class PropertiesWidget : public QWidget {
    Q_OBJECT
public:
    explicit PropertiesWidget(QWidget *parent = nullptr);
};
} // namespace qt::dock
