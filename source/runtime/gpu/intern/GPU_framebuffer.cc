#include "../GPU_framebuffer.h"
#include <QOpenGLFunctions_4_1_Core>
#include "../../creator_global.h"

namespace vektor::gpu {

GPUFramebuffer *GPU_framebuffer_create()
{
  auto *fbo = new GPUFramebuffer();
  fbo->depth_tex = nullptr;
  fbo->color_tex = nullptr;
  fbo->opengl_fbo = 0;
  fbo->metal_pass_descriptor = nullptr;

  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glGenFramebuffers(1, &fbo->opengl_fbo);
  } else {
#ifdef __APPLE__
    // Metal uses render pass descriptors directly
#endif
  }
  return fbo;
}

void GPU_framebuffer_free(GPUFramebuffer *fbo)
{
  if (!fbo) return;
  if (fbo->opengl_fbo) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glDeleteFramebuffers(1, &fbo->opengl_fbo);
  }
  delete fbo;
}

void GPU_framebuffer_bind(GPUFramebuffer *fbo)
{
  if (!fbo) return;
  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glBindFramebuffer(GL_FRAMEBUFFER, fbo->opengl_fbo);
    
    if (fbo->depth_tex) {
      gl.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depth_tex->opengl_id, 0);
    }
    if (!fbo->color_tex) {
      gl.glDrawBuffer(GL_NONE);
      gl.glReadBuffer(GL_NONE);
    }
  }
}

void GPU_framebuffer_unbind()
{
  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}

} // namespace vektor::gpu
