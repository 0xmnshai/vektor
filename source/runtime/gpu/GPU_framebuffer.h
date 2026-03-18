#pragma once

#include "GPU_texture.h"

namespace vektor::gpu {

typedef struct GPUFramebuffer {
  GPUTexture *depth_tex;
  GPUTexture *color_tex;
  
  // Backend handle
  unsigned int opengl_fbo;
  void *metal_pass_descriptor; // MTLRenderPassDescriptor*
} GPUFramebuffer;

GPUFramebuffer *GPU_framebuffer_create();
void GPU_framebuffer_free(GPUFramebuffer *fbo);
void GPU_framebuffer_bind(GPUFramebuffer *fbo);
void GPU_framebuffer_unbind();

} // namespace vektor::gpu
