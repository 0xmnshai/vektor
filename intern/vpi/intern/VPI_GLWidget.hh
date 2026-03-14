#pragma once

#include <QEvent>
#include <QWidget>
#include <QtOpenGLWidgets/QtOpenGLWidgets>

namespace vpi {
class VPI_GLWidget : public QOpenGLWidget {
  Q_OBJECT
 public:
  explicit VPI_GLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}

  ~VPI_GLWidget() override = default;

  // from QT: QOpenGLWidget
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  bool event(QEvent *event) override;
};
}  // namespace vpi
