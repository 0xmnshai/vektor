#pragma once
#include "GPU_texture.h"

namespace vektor::gpu {

typedef struct GPUFrameBuffer {
  unsigned int opengl_id;
  GPUTexture *depth_tex;
  int width, height;
} GPUFrameBuffer;

/** Create a framebuffer with only a depth attachment (for shadow mapping). */
GPUFrameBuffer *GPU_framebuffer_create_depth_only(int width, int height);
GPUFrameBuffer *GPU_framebuffer_create_depth_array(int width, int height, int layers);

/** Bind a specific layer of a depth array texture to the framebuffer. */
void GPU_framebuffer_attach_depth_layer(GPUFrameBuffer *fb, int layer);

/** Bind the framebuffer for rendering. */
void GPU_framebuffer_bind(GPUFrameBuffer *fb);

/** Unbind and return to the default framebuffer. */
void GPU_framebuffer_unbind();

/** Free the framebuffer and its attachments. */
void GPU_framebuffer_free(GPUFrameBuffer *fb);

} // namespace vektor::gpu
