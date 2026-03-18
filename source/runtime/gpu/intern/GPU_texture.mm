#include <QOpenGLFunctions_4_1_Core>
#ifdef __APPLE__
#  include "../../../intern/vpi/intern/VPI_ContextMTL.hh"
#  import <Metal/Metal.h>
#endif

#include "../../creator_global.h"
#include "../GPU_texture.h"

namespace vektor::gpu {

// TODO: use this function to create texture buffer
GPUTexture *GPU_texture_create_2d(int width, int height, eGPUTextureFormat format)
{
  auto *tex = new GPUTexture();
  tex->width = width;
  tex->height = height;
  tex->format = format;
  tex->opengl_id = 0;
  tex->metal_texture = nullptr;

  if (creator::G.gpu_backend == creator::GPU_BACKEND_OPENGL) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();

    gl.glGenTextures(1, &tex->opengl_id);
    gl.glBindTexture(GL_TEXTURE_2D, tex->opengl_id);

    if (format == GPU_DEPTH_COMPONENT24) {
      gl.glTexImage2D(GL_TEXTURE_2D,
                      0,
                      GL_DEPTH_COMPONENT24,
                      width,
                      height,
                      0,
                      GL_DEPTH_COMPONENT,
                      GL_FLOAT,
                      nullptr);
      gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else {
      gl.glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
      gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
  }
  else {
#ifdef __APPLE__
    auto device = (id<MTLDevice>)vpi::VPI_ContextMTL::get_current_device();
    MTLTextureDescriptor *texDesc = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:(format == GPU_DEPTH_COMPONENT24 ?
                                                MTLPixelFormatDepth32Float :
                                                MTLPixelFormatRGBA8Unorm)
                                     width:width
                                    height:height
                                 mipmapped:NO];
    texDesc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    texDesc.storageMode = MTLStorageModePrivate;
    tex->metal_texture = (void *)[device newTextureWithDescriptor:texDesc];
#endif
  }
  return tex;
}

void GPU_texture_free(GPUTexture *tex)
{
  if (!tex)
    return;
  if (tex->opengl_id) {
    QOpenGLFunctions_4_1_Core gl;
    gl.initializeOpenGLFunctions();
    gl.glDeleteTextures(1, &tex->opengl_id);
  }
#ifdef __APPLE__
  if (tex->metal_texture) {
    [(id<MTLTexture>)tex->metal_texture release];
  }
#endif
  delete tex;
}

}  // namespace vektor::gpu
