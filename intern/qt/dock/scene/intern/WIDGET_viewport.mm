#include <QSizePolicy>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../../../../../source/editor/windowmanager/wm_event_types.h"
#include "../../../../../source/runtime/dna/DNA_camera.h"
#include "../../../../../source/runtime/dna/DNA_object_type.h"
#include "../../../../../source/runtime/draw/DRW_manager.hh"
#include "../../../../../source/runtime/gpu/shaders/SHDR_grid.h"
#include "../../../../../source/runtime/lib/intern/appdir.h"
#include "../../../../source/runtime/gpu/GPU_shader.h"
#include "../../../../source/runtime/kernel/ecs/ECS_mesh_primitives.h"
#include "../../../../source/runtime/kernel/ecs/ECS_registry.h"
#include "../../../../source/runtime/lib/VLI_math_geom.h"
#include "../../../../source/runtime/rna/RNA_ecs_registry.h"
#include "../../../../vpi/intern/VPI_ContextMTL.hh"
#include "../../../../vpi/intern/VPI_QtWindow.hh"
#include "../../intern/qt/dock/scene/SCN_setup.h"
#include "../../intern/vpi/VPI_Types.h"
#include "../SCN_notifier.h"
#include "../WIDGET_viewport.h"

#ifdef __APPLE__
#  import <Metal/Metal.h>
#  import <QuartzCore/QuartzCore.h>
#endif

#define float_to_int(x) ((int)((x) + 0.5f))

// TODO: Thinking to create a separate file for openGL for this, let's see in blender later, how
// they are doing this. ?

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

  connect(&timer_, &QTimer::timeout, this, [this] {
    update_camera();
    update();
  });
  timer_.start(16);
}

void ViewportWidget::paintGL()
{
  // Initialize OpenGL functions FIRST before any GL calls or shader/buffer creation
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_OPENGL) {
    initializeOpenGLFunctions();
  }

  qt::scene::SCN_init_default_scene();

  if (!grid_initialized_ && grid_shader_) {
    QString shader_path = QString(vektor::lib::get_application_dir_path()) +
                          "/../../source/runtime/gpu/shaders/core/grid";
    grid_shader_->init(shader_path);
    grid_initialized_ = true;
  }

  // removed static meshes
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    if (context_) {
      auto *mtl_context = dynamic_cast<vpi::VPI_ContextMTL *>(context_);
      ::vektor::draw::DRW_prepare_view(nullptr);
      mtl_context->begin_render_pass();

      glm::mat4 projection = camera_->projection_matrix((float)width() / (float)height());
      glm::mat4 view = camera_->view_matrix();

      if (grid_shader_) {
        mtl_context->set_depth_write_enabled(false);
        grid_shader_->draw(projection, view);
        mtl_context->set_depth_write_enabled(true);
      }

      auto encoder = (id<MTLRenderCommandEncoder>)mtl_context->get_current_command_encoder();
      // Metal rendering for ECS objects via Draw Manager
      static float time = 0.0f;
      time += 0.016f;
      auto dpr = (float)devicePixelRatio();
      vektor::draw::DRW_draw_view(
          nullptr, encoder, view, projection, (width() * (int)dpr), (height() * (int)dpr), time);

      mtl_context->end_render_pass();
    }
    return;
  }

  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  vpi::VPI_GLWidget::paintGL();

  float aspect = (float)width() / (float)height();
  glm::mat4 projection = camera_->projection_matrix(aspect);
  glm::mat4 view = camera_->view_matrix();

  ::vektor::draw::DRW_prepare_view(nullptr);

  // CRITICAL: Restore viewport after shadow pass prepare
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl_func;
    gl_func.initializeOpenGLFunctions();
    float dpr = (float)devicePixelRatio();
    gl_func.glViewport(0, 0, width() * dpr, height() * dpr);
  }

  if (grid_shader_) {
    glDepthMask(GL_FALSE);
    grid_shader_->draw(projection, view);
    glDepthMask(GL_TRUE);
  }

  static float time = 0.0f;
  time += 0.016f;
  float dpr = (float)devicePixelRatio();
  vektor::draw::DRW_draw_view(
      nullptr, nullptr, view, projection, (width() * (int)dpr), (height() * (int)dpr), time);
  glDisable(GL_DEPTH_TEST);
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
      .x = (int32_t)event->position().x(),
      .y = (int32_t)event->position().y(),
      .button = (uint32_t)wm_event,
      .modifiers = (uint32_t)event->modifiers(),
      .type = VPI_kMouseButton,
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseEvent mouse_event(window, mouse_event_data);
  (void)mouse_event.consume();
}

void ViewportWidget::mouseMoveEvent(QMouseEvent *event)
{
  VPI_TEventCursorData mouse_event_data = {
      .x = (int32_t)event->position().x(),
      .y = (int32_t)event->position().y(),
      .type = VPI_kCursorMove,
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseMoveEvent mouse_event(window, mouse_event_data);
  (void)mouse_event.consume();

  float dx = (float)event->position().x() - (float)last_mouse_pos.x();
  float dy = (float)event->position().y() - (float)last_mouse_pos.y();

  // fix here
  if (event->modifiers() & Qt::ShiftModifier) {
    if (event->buttons() & (Qt::MiddleButton | Qt::LeftButton | Qt::RightButton)) {
      camera_->pan(dx, dy);
      update();
    }
  }
  else if (event->buttons() & Qt::RightButton) {
    // Fly mode look
    camera_->fly(dx, dy);
    update();
  }
  else if (event->buttons() & Qt::MiddleButton) {
    if (event->modifiers() & Qt::ControlModifier) {
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
    if (right_shift_down_) {
      camera_->pan((float)event->pixelDelta().x() * 0.5f, (float)event->pixelDelta().y() * 0.5f);
    }
    else {
      camera_->fly((float)event->pixelDelta().x() * 0.2f, (float)event->pixelDelta().y() * 0.2f);
    }
  }

  VPI_TEventMouseWheelData mouse_wheel_event_data = {
      .delta_x = (int32_t)event->pixelDelta().x(),
      .delta_y = (int32_t)event->pixelDelta().y(),
      .modifiers = (uint32_t)event->modifiers(),
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseWheelEvent mouse_wheel_event(window, mouse_wheel_event_data);
  (void)mouse_wheel_event.consume();

  update();
}

void ViewportWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    glm::vec3 ray_origin, ray_dir;
    camera_->screen_to_ray(
        (float)event->pos().x(), (float)event->pos().y(), width(), height(), ray_origin, ray_dir);

    vektor::dna::Object *closest_obj = nullptr;
    entt::entity closest_entity = entt::null;
    float closest_dist = FLT_MAX;

    auto &registry_instance = vektor::kernel::ECSRegistry::instance();
    auto &registry = registry_instance.registry();
    auto objects_view = registry.view<vektor::dna::Object>();

    // Clear selection
    for (auto entity : objects_view) {
      vektor::rna::RNA_ecs_set_selected(&registry_instance, entity, false);
      vektor::rna::RNA_ecs_set_active(&registry_instance, entity, false);
      auto &obj = objects_view.get<vektor::dna::Object>(entity);
      obj.select_flag &= ~(vektor::dna::BASE_SELECTED | vektor::dna::BASE_ACTIVE);
    }

    for (auto entity : objects_view) {
      auto &obj = objects_view.get<vektor::dna::Object>(entity);

      if (obj.mesh) {
        // Calculate model matrix
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, obj.transform.location);
        model = glm::rotate(model, obj.transform.rotation.x, glm::vec3(1, 0, 0));
        model = glm::rotate(model, obj.transform.rotation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, obj.transform.rotation.z, glm::vec3(0, 0, 1));
        model = glm::scale(model, obj.transform.scale);

        // Inverse model matrix to bring ray into object space
        glm::mat4 inv_model = glm::inverse(model);

        // Transform ray origin and direction to object space
        glm::vec3 obj_ray_origin = glm::vec3(inv_model * glm::vec4(ray_origin, 1.0f));
        glm::vec3 obj_ray_dir = glm::normalize(glm::vec3(inv_model * glm::vec4(ray_dir, 0.0f)));

        float t = vektor::lib::ray_mesh_intersect(obj_ray_origin, obj_ray_dir, obj.mesh.get());

        if (t != FLT_MAX) {
          // Transform hit distance back to world space for depth comparison
          glm::vec3 world_hit_pt = glm::vec3(model * glm::vec4(obj_ray_origin + obj_ray_dir * t, 1.0f));
          float world_t = glm::length(world_hit_pt - ray_origin);

          if (world_t < closest_dist) {
            closest_dist = world_t;
            closest_obj = &obj;
            closest_entity = entity;
          }
        }
      }
    }

    if (closest_obj) {
      vektor::rna::RNA_ecs_set_selected(&registry_instance, closest_entity, true);
      vektor::rna::RNA_ecs_set_active(&registry_instance, closest_entity, true);
      closest_obj->select_flag |= (vektor::dna::BASE_SELECTED | vektor::dna::BASE_ACTIVE);
    }

    outliner_notify_scene_changed();
  }
  else if (event->button() == Qt::RightButton) {
    right_mouse_down_ = true;
    setCursor(Qt::BlankCursor);
  }

  last_mouse_pos = event->pos();

  setFocus();

  VPI_TEventMouseButtonData mouse_button_event_data = {
      .x = (int32_t)event->pos().x(),
      .y = (int32_t)event->pos().y(),
      .button = (uint32_t)event->button(),
      .modifiers = (uint32_t)event->modifiers(),
      .type = VPI_kMouseButton,
  };

  auto *window = dynamic_cast<vpi::VPI_QtWindow *>(context_->get_window());
  vpi::VPI_MouseEvent mouse_button_event(window, mouse_button_event_data);
  (void)mouse_button_event.consume();
}

void ViewportWidget::keyPressEvent(QKeyEvent *event)
{
  keys_[event->key()] = true;
  if (event->key() == Qt::Key_Shift && event->nativeVirtualKey() == 0x3C) {
    right_shift_down_ = true;
  }
}

void ViewportWidget::keyReleaseEvent(QKeyEvent *event)
{
  keys_[event->key()] = false;
  if (event->key() == Qt::Key_Shift && event->nativeVirtualKey() == 0x3C) {
    right_shift_down_ = false;
  }
}

void ViewportWidget::update_camera()
{
  float move_speed = 0.1f;
  if (keys_[Qt::Key_Shift]) {
    move_speed *= 2.0f;
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
  if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
    pinch_Triggered(dynamic_cast<QPinchGesture *>(pinch));
    event->accept(Qt::PinchGesture);
  }
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
