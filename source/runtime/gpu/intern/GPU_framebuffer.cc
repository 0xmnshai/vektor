#include <QOpenGLContext>
#include <QOpenGLFunctions_4_1_Core>
#include "../creator_global.h"
#include "GPU_framebuffer.h"

namespace vektor::gpu {

GPUFrameBuffer *GPU_framebuffer_create_depth_array(int width, int height, int layers)
{
  auto *fb = new GPUFrameBuffer();
  fb->width = width;
  fb->height = height;
  fb->opengl_id = 0;
  fb->depth_tex = GPU_texture_create_2d_array(width, height, layers, GPU_DEPTH_COMPONENT24);

  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();

    gl.glGenFramebuffers(1, &fb->opengl_id);
    gl.glBindFramebuffer(GL_FRAMEBUFFER, fb->opengl_id);
    // Bind the first layer initially
    gl.glFramebufferTextureLayer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fb->depth_tex->opengl_id, 0, 0);

    gl.glDrawBuffer(GL_NONE);
    gl.glReadBuffer(GL_NONE);

    gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  return fb;
}

void GPU_framebuffer_attach_depth_layer(GPUFrameBuffer *fb, int layer)
{
  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glBindFramebuffer(GL_FRAMEBUFFER, fb->opengl_id);
    gl.glFramebufferTextureLayer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fb->depth_tex->opengl_id, 0, layer);
  }
}

GPUFrameBuffer *GPU_framebuffer_create_depth_only(int width, int height)
{
  auto *fb = new GPUFrameBuffer();
  fb->width = width;
  fb->height = height;
  fb->opengl_id = 0;
  fb->depth_tex = GPU_texture_create_2d(width, height, GPU_DEPTH_COMPONENT24);

  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();

    gl.glGenFramebuffers(1, &fb->opengl_id);
    gl.glBindFramebuffer(GL_FRAMEBUFFER, fb->opengl_id);
    gl.glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb->depth_tex->opengl_id, 0);

    // No color buffer for depth-only FBO
    gl.glDrawBuffer(GL_NONE);
    gl.glReadBuffer(GL_NONE);

    GLenum status = gl.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      // Error handling...
    }
    gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  return fb;
}

void GPU_framebuffer_bind(GPUFrameBuffer *fb)
{
  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    if (fb) {
      gl.glBindFramebuffer(GL_FRAMEBUFFER, fb->opengl_id);
      gl.glViewport(0, 0, fb->width, fb->height);
    }
    else {
      GLuint default_fbo = 0;
      auto *ctx = QOpenGLContext::currentContext();
      if (ctx) {
        default_fbo = ctx->defaultFramebufferObject();
      }
      gl.glBindFramebuffer(GL_FRAMEBUFFER, default_fbo);
    }
  }
}

void GPU_framebuffer_unbind()
{
  GPU_framebuffer_bind(nullptr);
}

void GPU_framebuffer_free(GPUFrameBuffer *fb)
{
  if (!fb) return;
  if (fb->opengl_id) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glDeleteFramebuffers(1, &fb->opengl_id);
  }
  if (fb->depth_tex) {
    GPU_texture_free(fb->depth_tex);
  }
  delete fb;
}

} // namespace vektor::gpu
