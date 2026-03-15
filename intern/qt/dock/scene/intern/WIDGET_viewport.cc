#include <QSizePolicy>
#include <glm/gtc/matrix_transform.hpp>

#include "../../../../../source/editor/windowmanager/wm_event_types.h"
#include "../../../../../source/runtime/gpu/shaders/SHDR_grid.h"
#include "../../../../vpi/intern/VPI_ContextMTL.hh"
#include "../../../../vpi/intern/VPI_QtWindow.hh"
#include "../../intern/vpi/VPI_Types.h"
#include "../WIDGET_viewport.h"

namespace qt::dock {

ViewportWidget::ViewportWidget(QWidget *parent) : vpi::VPI_GLWidget(parent)
{
  camera_ = new vektor::rna::Camera();
  camera_->set_camera_type(vektor::dna::DNA_CameraType::CAM_PERSP);
}

void ViewportWidget::init()
{
  vpi::VPI_GLWidget::init();
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMinimumSize(400, 300);

  setFocusPolicy(Qt::StrongFocus);
  grabGesture(Qt::PinchGesture);

  grid_shader_ = new vektor::gpu::GridShader();
  QString shader_path = QCoreApplication::applicationDirPath() +
                        "/../../source/runtime/gpu/shaders";
  grid_shader_->init(shader_path);

  connect(&timer_, &QTimer::timeout, this, [this] {
    update_camera();
    update();
  });
  timer_.start(16);
}

void ViewportWidget::paintGL()
{
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    if (context_) {
      auto *mtl_context = dynamic_cast<vpi::VPI_ContextMTL *>(context_);
      mtl_context->begin_render_pass();

      if (grid_shader_) {
        glm::mat4 projection = camera_->projection_matrix((float)width() / (float)height());
        glm::mat4 view = camera_->view_matrix();

        grid_shader_->draw(projection, view);
      }

      mtl_context->end_render_pass();
    }
    return;
  }

  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  vpi::VPI_GLWidget::paintGL();

  if (grid_shader_) {
    float aspect = (float)width() / (float)height();

    glm::mat4 projection = camera_->projection_matrix(aspect);
    glm::mat4 view = camera_->view_matrix();

    grid_shader_->draw(projection, view);
  }
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent *event)
{
  vektor::wmEventType wm_event = vektor::wmEventType::EVT_UNKNOWNKEY;

  switch (event->button()) {
    case Qt::LeftButton:
      wm_event = vektor::wmEventType::LEFTMOUSE;
      break;
    case Qt::RightButton:
      wm_event = vektor::wmEventType::RIGHTMOUSE;
      right_mouse_down_ = false;
      setCursor(Qt::ArrowCursor);
      break;
    case Qt::MiddleButton:
      wm_event = vektor::wmEventType::MIDDLEMOUSE;
      break;
    default:
      break;
  }

  VPI_TEventMouseButtonData mouse_event_data = {
      .x = (int)event->position().x(),
      .y = (int)event->position().y(),
      .button = (uint32_t)wm_event,
      .modifiers = event->modifiers(),
      .type = VPI_kMouseButton,
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseEvent mouse_event(window, mouse_event_data);
  (void)mouse_event.consume();
}

void ViewportWidget::mouseMoveEvent(QMouseEvent *event)
{
  VPI_TEventCursorData mouse_event_data = {
      .x = (int)event->position().x(),
      .y = (int)event->position().y(),
      .type = VPI_kCursorMove,
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseMoveEvent mouse_event(window, mouse_event_data);
  (void)mouse_event.consume();

  float dx = (float)event->position().x() - (float)last_mouse_pos.x();
  float dy = (float)event->position().y() - (float)last_mouse_pos.y();

  if (event->buttons() & Qt::RightButton) {
    // Fly mode look
    camera_->fly(dx, dy);
    update();
  }
  else if (event->buttons() & Qt::MiddleButton) {
    if (event->modifiers() & Qt::ShiftModifier) {
      // Pan (Move pivot)
      camera_->pan(dx, dy);
    }
    else if (event->modifiers() & Qt::ControlModifier) {
      // Zoom (Smooth)
      camera_->zoom(dy);
    }
    else {
      // Orbit
      camera_->orbit(dx, dy);
    }
    update();
  }
  last_mouse_pos = event->pos();
}

void ViewportWidget::wheelEvent(QWheelEvent *event)
{
  float wheel_y = (float)event->angleDelta().y() / 120.0f;
  float wheel_x = (float)event->angleDelta().x() / 120.0f;

  if (event->pixelDelta().isNull()) {
    // Discrete wheel (Mouse)
    float delta = (float)event->angleDelta().y() * 0.005f;
    float old_dist = camera_->distance();

    glm::vec3 mouse_world = camera_->get_world_point_on_pivot_plane(
        (float)event->position().x(), (float)event->position().y(), width(), height());

    camera_->set_distance(old_dist - delta);

    float zoom_factor = camera_->distance() / old_dist;
    camera_->set_pivot(mouse_world + (camera_->pivot() - mouse_world) * zoom_factor);
  }
  else {
    // Continuous scroll (Trackpad)
    camera_->fly((float)event->pixelDelta().x() * 0.2f, (float)event->pixelDelta().y() * 0.2f);
  }

  VPI_TEventMouseWheelData mouse_wheel_event_data = {
      .delta_x = event->pixelDelta().x(),
      .delta_y = event->pixelDelta().y(),
      .modifiers = event->modifiers(),
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseWheelEvent mouse_wheel_event(window, mouse_wheel_event_data);
  (void)mouse_wheel_event.consume();

  update();
}

void ViewportWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    glm::vec3 rayOrigin, rayDir;
    camera_->screen_to_ray(
        (float)event->pos().x(), (float)event->pos().y(), width(), height(), rayOrigin, rayDir);
  }
  else if (event->button() == Qt::RightButton) {
    right_mouse_down_ = true;
    setCursor(Qt::BlankCursor);
    last_mouse_pos = event->pos();
  }
  else if (event->button() == Qt::MiddleButton) {
    last_mouse_pos = event->pos();
  }

  setFocus();

  VPI_TEventMouseButtonData mouse_button_event_data = {
      .x = event->pos().x(),
      .y = event->pos().y(),
      .button = (uint32_t)event->button(),
      .modifiers = event->modifiers(),
      .type = VPI_kMouseButton,
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseEvent mouse_button_event(window, mouse_button_event_data);
  (void)mouse_button_event.consume();
}

void ViewportWidget::keyPressEvent(QKeyEvent *event)
{
  keys_[event->key()] = true;
}

void ViewportWidget::keyReleaseEvent(QKeyEvent *event)
{
  keys_[event->key()] = false;
}

void ViewportWidget::update_camera()
{
  float move_speed = 0.1f;
  if (keys_[Qt::Key_Shift]) {
    move_speed *= 5.0f;
  }

  glm::vec3 forward = camera_->get_forward_vector();
  glm::vec3 right = camera_->get_right_vector();
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  camera_->move(forward,
                right,
                up,
                keys_[Qt::Key_W],
                keys_[Qt::Key_S],
                keys_[Qt::Key_A],
                keys_[Qt::Key_D],
                keys_[Qt::Key_Q],
                keys_[Qt::Key_E],
                move_speed);
}

bool ViewportWidget::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture)
    return gesture_event(dynamic_cast<QGestureEvent *>(event));
  return vpi::VPI_GLWidget::event(event);
}

bool ViewportWidget::gesture_event(QGestureEvent *event)
{
  if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    pinch_Triggered(dynamic_cast<QPinchGesture *>(pinch));
  return true;
}

void ViewportWidget::pinch_Triggered(QPinchGesture *gesture)
{
  if (gesture->changeFlags() & QPinchGesture::ScaleFactorChanged) {
    float factor = (float)gesture->scaleFactor();
    if (factor > 0) {
      float old_dist = camera_->distance();
      glm::vec3 mouse_world = camera_->get_world_point_on_pivot_plane(
          (float)gesture->centerPoint().x(), (float)gesture->centerPoint().y(), width(), height());

      camera_->set_distance(old_dist / factor);

      float zoom_factor = camera_->distance() / old_dist;
      camera_->set_pivot(mouse_world + (camera_->pivot() - mouse_world) * zoom_factor);
      update();
    }
  }
}

}  // namespace qt::dock
