#pragma once
#include <QWidget>

namespace qt::dock {
class OutlinerWidget : public QWidget {
    Q_OBJECT
public:
    explicit OutlinerWidget(QWidget *parent = nullptr);
};
} // namespace qt::dock
