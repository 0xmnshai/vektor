#include "PRP_light.hh"
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "../../scene/SCN_notifier.h"

namespace qt::dock {

LightPanel::LightPanel(vektor::dna::Object *ob, QWidget *parent)
    : PropertySubPanel("Light", ob, parent)
{
  int row = 0;
  
  // Color
  auto *color_label = new QLabel("Color");
  color_button_ = new QPushButton();
  color_button_->setFixedWidth(40);
  layout_->addWidget(color_label, row, 0);
  layout_->addWidget(color_button_, row, 1);
  row++;

  // Intensity
  auto *energy_label = new QLabel("Intensity");
  intensity_slider_ = new QSlider(Qt::Horizontal);
  intensity_slider_->setRange(0, 1000);
  energy_sb_ = new DragSpinBox();
  energy_sb_->setRange(0.0, 10000.0);
  energy_sb_->setSuffix(" W");
  
  layout_->addWidget(energy_label, row, 0);
  layout_->addWidget(intensity_slider_, row, 1, 1, 2);
  layout_->addWidget(energy_sb_, row, 3);
  row++;

  // Range
  auto *range_label = new QLabel("Range");
  range_sb_ = new DragSpinBox();
  range_sb_->setRange(0.1, 1000.0);
  range_sb_->setSuffix(" m");
  
  layout_->addWidget(range_label, row, 0);
  layout_->addWidget(range_sb_, row, 1);
  row++;

  // Connections
  connect(color_button_, &QPushButton::clicked, this, &LightPanel::on_color_clicked);
  connect(intensity_slider_, &QSlider::valueChanged, this, &LightPanel::on_intensity_slider_changed);
  
  auto prop_changed = [this]() { on_property_changed(); };
  connect(energy_sb_, QOverload<double>::of(&DragSpinBox::valueChanged), prop_changed);
  connect(range_sb_, QOverload<double>::of(&DragSpinBox::valueChanged), prop_changed);

  update_ui();
}

void LightPanel::update_ui()
{
  if (!object_ || !object_->light) return;
  auto &la = *object_->light;

  QColor col = QColor::fromRgbF(la.color.r, la.color.g, la.color.b);
  color_button_->setStyleSheet(
      QString("background-color: %1; border: 1px solid #555;").arg(col.name()));

  intensity_slider_->blockSignals(true);
  intensity_slider_->setValue((int)(la.energy * 100.0f));
  intensity_slider_->blockSignals(false);

  energy_sb_->blockSignals(true);
  energy_sb_->setValue(la.energy);
  energy_sb_->blockSignals(false);

  range_sb_->blockSignals(true);
  range_sb_->setValue(la.distance);
  range_sb_->blockSignals(false);
}

void LightPanel::on_color_clicked()
{
  auto &la = *object_->light;
  QColor col = QColorDialog::getColor(
      QColor::fromRgbF(la.color.r, la.color.g, la.color.b), this, "Light Color");
  if (col.isValid()) {
    la.color = glm::vec3(col.redF(), col.greenF(), col.blueF());
    update_ui();
    qt::scene::SCN_notifier::instance()->notifySceneChanged();
  }
}

void LightPanel::on_intensity_slider_changed(int value)
{
  object_->light->energy = (float)value / 100.0f;
  update_ui();
  qt::scene::SCN_notifier::instance()->notifySceneChanged();
}

void LightPanel::on_property_changed()
{
  auto &la = *object_->light;
  la.energy = (float)energy_sb_->value();
  la.distance = (float)range_sb_->value();
  la.type = vektor::dna::LA_LOCAL;

  update_ui();
  qt::scene::SCN_notifier::instance()->notifySceneChanged();
}

}  // namespace qt::dock
