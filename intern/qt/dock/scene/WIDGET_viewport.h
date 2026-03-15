#pragma once

#include <QWidget>

#include <QGestureEvent>
#include <QPinchGesture>
#include <QTimer>
#include <map>

#include <QOpenGLFunctions_4_1_Core>

#include "../../../../intern/vpi/intern/VPI_GLWidget.hh"
#include "../../../../source/runtime/gpu/shaders/SHDR_grid.h"
#include "../../../../source/runtime/rna/RNA_camera.h"
#include <glm/gtc/type_ptr.hpp>

namespace qt::dock {
class ViewportWidget : public vpi::VPI_GLWidget, protected QOpenGLFunctions_4_1_Core {
 public:
  explicit ViewportWidget(QWidget *parent = nullptr);
  ~ViewportWidget() override = default;

  void init() override;

  void paintGL() override;

  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

  bool event(QEvent *event) override;
  bool gesture_event(QGestureEvent *event);
  void pinch_Triggered(QPinchGesture *gesture);

 private:
  void update_camera();
  vektor::gpu::GridShader *grid_shader_ = nullptr;

  bool right_mouse_down_ = false;
  QPoint last_mouse_pos;

  vektor::rna::Camera *camera_ = nullptr;
  std::map<int, bool> keys_;
  QTimer timer_;

  // Cylinder rendering cache
  GLuint cylinder_vao_ = 0;
  GLuint cylinder_vbo_ = 0;
  int cylinder_vertex_count_ = 0;
  bool mesh_initialized_ = false;
  void init_cylinder_mesh();
};
}  // namespace qt::dock
