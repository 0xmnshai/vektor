#include <QSizePolicy>
#include <glm/gtc/matrix_transform.hpp>

#include "../../../../../source/editor/windowmanager/wm_event_types.h"
#include "../../../../../source/runtime/dna/DNA_camera.h"
#include "../../../../../source/runtime/dna/DNA_object_type.h"
#include "../../../../../source/runtime/gpu/shaders/SHDR_grid.h"
#include "../../../../../source/runtime/lib/intern/appdir.h"
#include "../../../../source/runtime/gpu/GPU_shader.h"
#include "../../../../source/runtime/kernel/ecs/ECS_mesh_primitives.h"
#include "../../../../source/runtime/kernel/ecs/ECS_registry.h"
#include "../../../../vpi/intern/VPI_ContextMTL.hh"
#include "../../../../vpi/intern/VPI_QtWindow.hh"
#include "../../intern/vpi/VPI_Types.h"
#include "../SCN_setup.h"
#include "../WIDGET_viewport.h"

#ifdef __APPLE__
#  import <Metal/Metal.h>
#  import <QuartzCore/QuartzCore.h>
#endif

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

  timer_.start(16);
}

void ViewportWidget::paintGL()
{
  qt::scene::SCN_init_default_scene();

  if (!grid_initialized_ && grid_shader_) {
    QString shader_path = QString(vektor::lib::get_application_dir_path()) +
                          "/../../source/runtime/gpu/shaders/file/grid";
    grid_shader_->init(shader_path);
    grid_initialized_ = true;
  }

  if (!mesh_initialized_) {
    init_cylinder_mesh();
  }

  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    if (context_) {
      auto *mtl_context = dynamic_cast<vpi::VPI_ContextMTL *>(context_);
      mtl_context->begin_render_pass();

      if (grid_shader_) {
        glm::mat4 projection = camera_->projection_matrix((float)width() / (float)height());
        glm::mat4 view = camera_->view_matrix();

        grid_shader_->draw(projection, view);
      }

      // Metal rendering for ECS objects
      auto &registry = vektor::kernel::ECSRegistry::instance().registry();
      auto objects_view = registry.view<vektor::dna::Object>();

      auto encoder = (id<MTLRenderCommandEncoder>)mtl_context->get_current_command_encoder();

      for (auto entity : objects_view) {
        auto &obj = objects_view.get<vektor::dna::Object>(entity);

        if (obj.type == vektor::dna::DNA_ENTITY_CYLINDER && obj.shader_program) {
          auto *gpu_shader = (vektor::gpu::GPUShader *)obj.shader_program;
          auto pipeline = (id<MTLRenderPipelineState>)gpu_shader->metal_pipeline;

          if (pipeline && cylinder_metal_vbo_) {
            [encoder setRenderPipelineState:pipeline];
            [encoder setVertexBuffer:(id<MTLBuffer>)cylinder_metal_vbo_ offset:0 atIndex:0];

            struct Uniforms {
              glm::mat4 model;
              glm::mat4 view;
              glm::mat4 projection;
            };

            Uniforms uniforms = {};
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, obj.transform.location);
            model = glm::scale(model, obj.transform.scale);

            uniforms.model = model;
            uniforms.view = camera_->view_matrix();
            uniforms.projection = camera_->projection_matrix((float)width() / (float)height());

            [encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];

            // Material and light params
            glm::vec4 color = obj.material.color;
            glm::vec4 lightPos(10.0f, 10.0f, 10.0f, 1.0f);
            glm::vec4 viewPos(camera_->eye_position(), 1.0f);

            [encoder setFragmentBytes:&color length:sizeof(color) atIndex:0];
            [encoder setFragmentBytes:&lightPos length:sizeof(lightPos) atIndex:2];
            [encoder setFragmentBytes:&viewPos length:sizeof(viewPos) atIndex:3];

            [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                        vertexStart:0
                        vertexCount:cylinder_vertex_count_];
          }
        }
      }

      mtl_context->end_render_pass();
    }
    return;
  }

  initializeOpenGLFunctions();
  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  vpi::VPI_GLWidget::paintGL();

  float aspect = (float)width() / (float)height();
  glm::mat4 projection = camera_->projection_matrix(aspect);
  glm::mat4 view = camera_->view_matrix();

  if (grid_shader_) {
    grid_shader_->draw(projection, view);
  }

  // Render ECS objects
  auto &registry = vektor::kernel::ECSRegistry::instance().registry();
  auto objects_view = registry.view<vektor::dna::Object>();

  for (auto entity : objects_view) {
    auto &obj = objects_view.get<vektor::dna::Object>(entity);

    if (obj.type == vektor::dna::DNA_ENTITY_CYLINDER && obj.shader_program) {
      auto *gpu_shader = (vektor::gpu::GPUShader *)obj.shader_program;
      vektor::gpu::GPU_shader_bind(gpu_shader);

      // Model matrix
      auto model = glm::mat4(1.0f);
      model = glm::translate(model, obj.transform.location);
      // rotation here ... scale here ...
      model = glm::scale(model, obj.transform.scale);

      vektor::gpu::GPU_shader_uniform_matrix4(gpu_shader, "model", glm::value_ptr(model));
      vektor::gpu::GPU_shader_uniform_matrix4(gpu_shader, "view", glm::value_ptr(view));
      vektor::gpu::GPU_shader_uniform_matrix4(
          gpu_shader, "projection", glm::value_ptr(projection));

      vektor::gpu::GPU_shader_uniform_float(
          gpu_shader, "color", obj.material.color.r);  // Simplified, original used vec4
      // Correctly set color vector
      auto *program = vektor::gpu::GPU_shader_get_program(gpu_shader);
      if (program) {
        program->setUniformValue("color",
                                 obj.material.color.r,
                                 obj.material.color.g,
                                 obj.material.color.b,
                                 obj.material.color.a);
        program->setUniformValue("lightPos", 10.0f, 10.0f, 10.0f);
        program->setUniformValue("viewPos",
                                 camera_->eye_position().x,
                                 camera_->eye_position().y,
                                 camera_->eye_position().z);
      }

      glBindVertexArray(cylinder_vao_);
      glDrawArrays(GL_TRIANGLES, 0, cylinder_vertex_count_);
      glBindVertexArray(0);

      vektor::gpu::GPU_shader_unbind(gpu_shader);
    }
  }

  glDisable(GL_DEPTH_TEST);
}

void ViewportWidget::init_cylinder_mesh()
{
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
#ifdef __APPLE__
    auto device = (id<MTLDevice>)vpi::VPI_ContextMTL::get_current_device();
    if (device) {
      std::vector<vektor::kernel::Vertex> vertices;
      vektor::kernel::create_cylinder_mesh(vertices, 1.0f, 2.0f, 32);
      cylinder_vertex_count_ = (int)vertices.size();

      id<MTLBuffer> buffer = [device
          newBufferWithBytes:vertices.data()
                      length:vertices.size() * sizeof(vektor::kernel::Vertex)
                     options:MTLResourceStorageModeShared];
      cylinder_metal_vbo_ = (void *)buffer;
      mesh_initialized_ = true;
    }
#endif
    return;
  }

  initializeOpenGLFunctions();
  std::vector<vektor::kernel::Vertex> vertices;
  vektor::kernel::create_cylinder_mesh(vertices, 1.0f, 2.0f, 32);
  cylinder_vertex_count_ = (int)vertices.size();

  glGenVertexArrays(1, &cylinder_vao_);
  glGenBuffers(1, &cylinder_vbo_);

  glBindVertexArray(cylinder_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, cylinder_vbo_);
  glBufferData(GL_ARRAY_BUFFER,
               (GLsizeiptr)(vertices.size() * sizeof(vektor::kernel::Vertex)),
               vertices.data(),
               GL_STATIC_DRAW);

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vektor::kernel::Vertex), (void *)0);

  // Normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, sizeof(vektor::kernel::Vertex), (void *)(sizeof(glm::vec3)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  mesh_initialized_ = true;
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
