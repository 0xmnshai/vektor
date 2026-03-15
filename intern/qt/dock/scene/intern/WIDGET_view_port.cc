#include <QSizePolicy>
#include <glm/gtc/matrix_transform.hpp>

#include "../../../../../source/runtime/gpu/shaders/SHDR_grid.h"
#include "../../../../vpi/intern/VPI_ContextMTL.hh"
#include "../WIDGET_view_port.h"

namespace qt::dock {

ViewportWidget::ViewportWidget(QWidget *parent) : vpi::VPI_GLWidget(parent) {}

void ViewportWidget::init()
{
  vpi::VPI_GLWidget::init();
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMinimumSize(400, 300);

  grid_shader_ = new vektor::gpu::GridShader();
  QString shader_path = QCoreApplication::applicationDirPath() +
                        "/../../source/runtime/gpu/shaders";
  grid_shader_->init(shader_path);
}

void ViewportWidget::paintGL()
{
  if (vektor::creator::G.gpu_backend == vektor::creator::GPU_BACKEND_METAL) {
    if (context_) {
      auto *mtl_context = dynamic_cast<vpi::VPI_ContextMTL *>(context_);
      mtl_context->begin_render_pass();

      if (grid_shader_) {
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), (float)width() / (float)height(), 0.01f, 1000.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(20.0f, 20.0f, 20.0f),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));

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
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)width() / (float)height(), 0.01f, 1000.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    grid_shader_->draw(projection, view);
  }
}
}  // namespace qt::dock
