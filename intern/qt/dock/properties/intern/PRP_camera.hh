#pragma once

#include "../PRP_subpanel.hh"


// TODO: first we will implement light

namespace qt::dock {
class CameraPanel : public PropertySubPanel {
  Q_OBJECT
public:
  CameraPanel(vektor::dna::Object *ob, QWidget *parent);

  // field of view - slider 

private:
  void update_ui() override;
};
}