#include "VPI_GLWidget.hh"

namespace vpi {
void VPI_GLWidget::initializeGL() {}

// we can shift widget specific things to specific widget class [name].cc
void VPI_GLWidget::resizeGL(int width, int height) {}

void VPI_GLWidget::paintGL() {}

void VPI_GLWidget::mousePressEvent(QMouseEvent *event) {}

void VPI_GLWidget::mouseReleaseEvent(QMouseEvent *event) {}

void VPI_GLWidget::keyPressEvent(QKeyEvent *event) {}

void VPI_GLWidget::keyReleaseEvent(QKeyEvent *event) {}

void VPI_GLWidget::wheelEvent(QWheelEvent *event) {}

bool VPI_GLWidget::event(QEvent *event)
{
  return QOpenGLWidget::event(event);
}
}  // namespace vpi
