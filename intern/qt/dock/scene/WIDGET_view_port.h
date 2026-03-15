#pragma once

#include <QWidget>

#include "../../../../intern/vpi/intern/VPI_GLWidget.hh"
#include "../../../../source/runtime/gpu/shaders/SHDR_grid.h"

namespace qt::dock {
class ViewportWidget : public vpi::VPI_GLWidget {
 public:
  explicit ViewportWidget(QWidget *parent = nullptr);
  ~ViewportWidget() override = default;

  void init() override;

  void paintGL() override;

 private:
  vektor::gpu::GridShader *grid_shader_ = nullptr;
};
}  // namespace qt::dock
