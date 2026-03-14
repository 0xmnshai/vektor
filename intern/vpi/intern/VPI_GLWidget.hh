#pragma once

#include <QEvent>
#include <QWidget>
#include <QtOpenGLWidgets/QtOpenGLWidgets>

#include "VPI_Context.hh"

namespace vpi {
#if defined(__APPLE__) || defined(METAL)
class VPI_GLWidget : public QWidget {
#else
class VPI_GLWidget : public QOpenGLWidget {
#endif
  Q_OBJECT
 public:
#if defined(__APPLE__) || defined(METAL)
  explicit VPI_GLWidget(QWidget *parent = nullptr) : QWidget(parent) {}
#else
  explicit VPI_GLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}
#endif

  ~VPI_GLWidget() override = default;

  virtual void init();

#if !defined(__APPLE__) && !defined(METAL)
  // from QT: QOpenGLWidget
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
#else
  void resizeGL(int width, int height);
#endif
  void resizeEvent(QResizeEvent *event) override;
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
};
}  // namespace vpi
