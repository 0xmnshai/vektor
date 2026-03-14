#include "VPI_GLWidget.hh"
#if defined(__APPLE__) || defined(METAL)
#  include "VPI_ContextMTL.hh"
#endif

namespace vpi {
#if !defined(__APPLE__) && !defined(METAL)
void VPI_GLWidget::initializeGL()
{
  init();
}
#endif

void VPI_GLWidget::init()
{
  if (context_) {
    return;
  }
#if defined(__APPLE__) || defined(METAL)
  setAttribute(Qt::WA_NativeWindow);
  setAttribute(Qt::WA_PaintOnScreen);
#endif
  init_vpi_context();
  setAttribute(Qt::WA_OpaquePaintEvent);
  setAttribute(Qt::WA_NoSystemBackground);
  if (context_) {
    (void)context_->init_context();
  }
}

void VPI_GLWidget::init_vpi_context()
{
  const VPI_ContextParams params = {false, false, VPI_kVSyncModeAuto};  // Default params
#if defined(__APPLE__) || defined(METAL)
  // For Metal, we might need the native view
  context_ = new VPI_ContextMTL(params, (void *)winId(), nullptr);
#else
  context_ = new VPI_ContextGL(params, nullptr);  // window_ is nullptr for now
#endif
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
#if defined(__APPLE__) || defined(METAL)
  resizeGL(event->size().width(), event->size().height());
#else
  QOpenGLWidget::resizeEvent(event);
#endif
}

#if !defined(__APPLE__) && !defined(METAL)
void VPI_GLWidget::paintGL() {}
#endif

void VPI_GLWidget::mousePressEvent(QMouseEvent *event) {}

void VPI_GLWidget::mouseReleaseEvent(QMouseEvent *event) {}

void VPI_GLWidget::keyPressEvent(QKeyEvent *event) {}

void VPI_GLWidget::keyReleaseEvent(QKeyEvent *event) {}

void VPI_GLWidget::wheelEvent(QWheelEvent *event) {}

bool VPI_GLWidget::event(QEvent *event)
{
#if defined(__APPLE__) || defined(METAL)
  return QWidget::event(event);
#else
  return QOpenGLWidget::event(event);
#endif
}
}  // namespace vpi
