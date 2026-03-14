#pragma once

#include <QWidget>

#include "../../../../intern/vpi/intern/VPI_GLWidget.hh"

namespace qt::dock {
class ViewportWidget : public vpi::VPI_GLWidget {
 public:
  explicit ViewportWidget(QWidget *parent = nullptr) : vpi::VPI_GLWidget(parent) {}
  ~ViewportWidget() override = default;

  void init() override;
};

}  // namespace qt::dock
