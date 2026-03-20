#include "PRP_transform.hh"
#include "../../scene/SCN_notifier.h"
#include <QtMath>

namespace qt::dock {
TransformPanel::TransformPanel(vektor::dna::Object *ob, QWidget *parent)
    : PropertySubPanel("Transform", ob, parent)
{
  create_spin_box_row(0, "Position", loc_x_, loc_y_, loc_z_, 0.1);
  create_spin_box_row(1, "Rotation", rot_x_, rot_y_, rot_z_, 1.0);
  create_spin_box_row(2, "Scale", scale_x_, scale_y_, scale_z_, 0.01);

  auto update_object = [this]() {
    object_->transform.location = {loc_x_->value(), loc_y_->value(), loc_z_->value()};
    object_->transform.rotation = {
        (float)qDegreesToRadians(rot_x_->value()),
        (float)qDegreesToRadians(rot_y_->value()),
        (float)qDegreesToRadians(rot_z_->value())};
    object_->transform.scale = {scale_x_->value(), scale_y_->value(), scale_z_->value()};

    // Notify scene changed
    qt::scene::SCN_notifier::instance()->notifySceneChanged();
  };

  connect(loc_x_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(loc_y_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(loc_z_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(rot_x_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(rot_y_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(rot_z_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(scale_x_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(scale_y_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);
  connect(scale_z_, qOverload<double>(&QDoubleSpinBox::valueChanged), update_object);

  update_ui();
}

void TransformPanel::update_ui()
{
  loc_x_->blockSignals(true);
  loc_y_->blockSignals(true);
  loc_z_->blockSignals(true);
  rot_x_->blockSignals(true);
  rot_y_->blockSignals(true);
  rot_z_->blockSignals(true);
  scale_x_->blockSignals(true);
  scale_y_->blockSignals(true);
  scale_z_->blockSignals(true);

  loc_x_->setValue(object_->transform.location.x);
  loc_y_->setValue(object_->transform.location.y);
  loc_z_->setValue(object_->transform.location.z);
  rot_x_->setValue(qRadiansToDegrees((double)object_->transform.rotation.x));
  rot_y_->setValue(qRadiansToDegrees((double)object_->transform.rotation.y));
  rot_z_->setValue(qRadiansToDegrees((double)object_->transform.rotation.z));
  scale_x_->setValue(object_->transform.scale.x);
  scale_y_->setValue(object_->transform.scale.y);
  scale_z_->setValue(object_->transform.scale.z);

  loc_x_->blockSignals(false);
  loc_y_->blockSignals(false);
  loc_z_->blockSignals(false);
  rot_x_->blockSignals(false);
  rot_y_->blockSignals(false);
  rot_z_->blockSignals(false);
  scale_x_->blockSignals(false);
  scale_y_->blockSignals(false);
  scale_z_->blockSignals(false);
}
}  // namespace qt::dock
