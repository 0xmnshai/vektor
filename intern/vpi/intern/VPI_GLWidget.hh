#pragma once

#include <QEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "VPI_Context.hh"
#include "../../../source/runtime/creator_global.h"

namespace vpi {
// On macOS, if we are using OpenGL, we MUST inherit from QOpenGLWidget.
// If we are using Metal, we use QWidget as a container for CAMetalLayer.
inline bool is_metal_backend() {
    return vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL;
}

class VPI_GLWidget : public QOpenGLWidget {
  Q_OBJECT
 public:
  explicit VPI_GLWidget(QWidget *parent = nullptr);

  ~VPI_GLWidget() override = default;

  virtual void init();

  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void showEvent(QShowEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  bool event(QEvent *event) override;

 protected:
  // graphics context ...
  void init_vpi_context();
  VPI_Context *context_ = nullptr;  // we will use this for metal_context or opengl_context
  bool initialized_ = false;
};
}  // namespace vpi
