#pragma once

namespace vektor::gpu {

typedef enum eGPUTextureFormat {
  GPU_DEPTH_COMPONENT24,
  GPU_RGBA8,
} eGPUTextureFormat;

typedef struct GPUTexture {
  int width, height;
  eGPUTextureFormat format;

  // Backend handle
  unsigned int opengl_id;
  void *metal_texture;  // id<MTLTexture>
} GPUTexture;

GPUTexture *GPU_texture_create_2d(int width, int height, eGPUTextureFormat format);
GPUTexture *GPU_texture_create_2d_array(int width, int height, int layers, eGPUTextureFormat format);
void GPU_texture_free(GPUTexture *tex);

}  // namespace vektor::gpu
