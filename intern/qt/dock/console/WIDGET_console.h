#pragma once
#include <QWidget>

namespace qt::dock {
class ConsoleWidget : public QWidget {
    Q_OBJECT
public:
    explicit ConsoleWidget(QWidget *parent = nullptr);
};
} // namespace qt::dock
