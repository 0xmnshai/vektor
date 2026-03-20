#pragma once

#include <QVBoxLayout>
#include <QWidget>

#include "../../../../source/runtime/dna/DNA_object_type.h"

namespace qt::dock {
class PropertiesPanel : public QWidget {
  Q_OBJECT
 public:
  explicit PropertiesPanel(QWidget *parent = nullptr);

 private slots:
  void on_selection_changed();

 private:
  vektor::dna::Object *selected_object_ = nullptr;

  QWidget *container_widget_ = nullptr;
  QVBoxLayout *sub_panel_layout_ = nullptr;
};
}  // namespace qt::dock
