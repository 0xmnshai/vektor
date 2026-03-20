#include "PRP_camera.hh"

namespace qt::dock {
CameraPanel::CameraPanel(vektor::dna::Object *ob, QWidget *parent)
    : PropertySubPanel("Camera", ob, parent)
{
  update_ui();
}

void CameraPanel::update_ui()
{
  if (!object_ || !object_->camera) {
    return;
  }
  // TODO: Implement camera specific UI later
}
}
