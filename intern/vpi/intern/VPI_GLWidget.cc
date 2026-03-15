#include "VPI_GLWidget.hh"
#include "../../../source/runtime/creator_global.h"
#include "VPI_ContextGL.hh"
#include "VPI_ContextMTL.hh"
#include "VPI_QtWindow.hh"

namespace vpi {
VPI_GLWidget::VPI_GLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

void VPI_GLWidget::paintEvent(QPaintEvent *event)
{
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_OPENGL) {
    QOpenGLWidget::paintEvent(event);
  }
  else if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    paintGL();
  }
}

void VPI_GLWidget::initializeGL()
{
  init();
}

void VPI_GLWidget::init()
{
  if (initialized_) {
    return;
  }
  init_vpi_context();

  setAttribute(Qt::WA_OpaquePaintEvent);
  setAttribute(Qt::WA_NoSystemBackground);

  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
  }

  initialized_ = true;
  if (context_) {
    (void)context_->init_context();
  }
}

void VPI_GLWidget::init_vpi_context()
{
  VPI_Window *window = nullptr;
  QWidget *parent = parentWidget();
  while (parent) {
    if (auto *vpi_win = qobject_cast<VPI_QtWindow *>(parent)) {
      window = vpi_win;
      break;
    }
    parent = parent->parentWidget();
  }

  const VPI_ContextParams params = {false, false, VPI_kVSyncModeAuto};  // Default params
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    // For Metal, we might need the native view
    context_ = new VPI_ContextMTL(params, window, (void *)winId(), nullptr);
  }
  else {
    context_ = new VPI_ContextGL(params, window);
  }
}

// we can shift widget specific things to specific widget class [name].cc
void VPI_GLWidget::resizeGL(int width, int height)
{
  if (context_) {
    context_->resize_context(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
  }
}

void VPI_GLWidget::resizeEvent(QResizeEvent *event)
{
  QOpenGLWidget::resizeEvent(event);
}

void VPI_GLWidget::showEvent(QShowEvent *event)
{
  if (!initialized_) {
    init();
  }
  QOpenGLWidget::showEvent(event);
}

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
