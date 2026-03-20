#pragma once

#include "../PRP_subpanel.hh"
#include "PRP_drag_spinbox.hh"

namespace qt::dock {
class TransformPanel : public PropertySubPanel {
  Q_OBJECT
 public:
  explicit TransformPanel(vektor::dna::Object *ob, QWidget *parent = nullptr);

 private:
  void update_ui() override;

  DragSpinBox *loc_x_ = nullptr, *loc_y_ = nullptr, *loc_z_ = nullptr;
  DragSpinBox *rot_x_ = nullptr, *rot_y_ = nullptr, *rot_z_ = nullptr;
  DragSpinBox *scale_x_ = nullptr, *scale_y_ = nullptr, *scale_z_ = nullptr;
};
}  // namespace qt::dock
