#pragma once

#include "../PRP_subpanel.hh"
#include <QComboBox>
#include <QSlider>

namespace qt::dock {

class LightPanel : public PropertySubPanel {
  Q_OBJECT
 public:
  explicit LightPanel(vektor::dna::Object *ob, QWidget *parent = nullptr);

 private:
  void update_ui() override;

  QPushButton *color_button_;
  DragSpinBox *energy_sb_;
  QSlider *intensity_slider_;
  DragSpinBox *range_sb_;
  
  void on_color_clicked();
  void on_intensity_slider_changed(int value);
  void on_property_changed();
};

}  // namespace qt::dock
