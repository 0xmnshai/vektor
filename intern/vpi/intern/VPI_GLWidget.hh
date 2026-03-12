#pragma once

#include <QWidget>
#include <QtOpenGLWidgets/QtOpenGLWidgets>

namespace vpi {
class VPI_GLWidget : public QOpenGLWidget {
 public:
  explicit VPI_GLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}
};
}  // namespace vpi
