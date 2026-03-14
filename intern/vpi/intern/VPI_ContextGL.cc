#include <QOpenGLBuffer>

#include "VPI_ContextGL.hh"
#include "VPI_Types.h"

namespace vpi {
VPI_ContextGL::VPI_ContextGL(const VPI_ContextParams &context_param, VPI_Window *window)
    : VPI_Context(context_param), window_(window)
{
}
 
VPI_ContextGL::~VPI_ContextGL() = default;

VPI_TSuccess VPI_ContextGL::init_context() const
{
  //   initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  return VPI_TSuccess::VPI_kSuccess;
}

VPI_TSuccess VPI_ContextGL::release_context() const
{
  return VPI_TSuccess::VPI_kSuccess;
}

VPI_TSuccess VPI_ContextGL::release_native_handles() const
{
  return VPI_TSuccess::VPI_kSuccess;
}
}  // namespace vpi
