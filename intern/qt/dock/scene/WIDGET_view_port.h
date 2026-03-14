#pragma once

#include <QWidget>

#include "../../../vpi/intern/VPI_GLWidget.hh"

namespace qt::dock {
class ViewportWidget : public vpi::VPI_GLWidget {
 public:
  explicit ViewportWidget(QWidget *parent = nullptr) : vpi::VPI_GLWidget(parent) {}
  ~ViewportWidget() override = default;


  
};
}  // namespace qt::dock
