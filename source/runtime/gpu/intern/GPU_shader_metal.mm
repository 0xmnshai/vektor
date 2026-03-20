#include "MEM_gaurdalloc.h"
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include "../../../intern/clog/CLG_log.h"
#include "../../../intern/vpi/intern/VPI_ContextMTL.hh"
#include "../GPU_shader.h"

namespace vektor::gpu {

void *GPU_metal_pipeline_create(const QByteArray &vert_code, const QByteArray &frag_code)
{
  auto device = (id<MTLDevice>)vpi::VPI_ContextMTL::get_current_device();
  if (!device) {
    CLOG_ERROR(LOG_SHADER, "[GPU_shader_metal] No Metal device available");
    return nullptr;
  }

  NSError *error = nil;

  dispatch_data_t vert_dispatch = dispatch_data_create(
      vert_code.constData(), vert_code.size(), nil, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
  id<MTLLibrary> vert_lib = [device newLibraryWithData:vert_dispatch error:&error];
  if (error) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader_metal] Vert library error: %s",
               [[error localizedDescription] UTF8String]);
    return nullptr;
  }

  dispatch_data_t frag_dispatch = dispatch_data_create(
      frag_code.constData(), frag_code.size(), nil, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
  id<MTLLibrary> frag_lib = [device newLibraryWithData:frag_dispatch error:&error];
  if (error) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader_metal] Frag library error: %s",
               [[error localizedDescription] UTF8String]);
    return nullptr;
  }

  id<MTLFunction> vert_func = [vert_lib newFunctionWithName:@"main"];
  id<MTLFunction> frag_func = [frag_lib newFunctionWithName:@"main"];

  if (!vert_func || !frag_func) {
    CLOG_ERROR(LOG_SHADER, "[GPU_shader_metal] Failed to find entry points ('main') in library");
    return nullptr;
  }

  MTLRenderPipelineDescriptor *desc = [[MTLRenderPipelineDescriptor alloc] init];
  desc.vertexFunction = vert_func;
  desc.fragmentFunction = frag_func;
  desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
  desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
  desc.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

  // Editor defaults: Enable blending
  desc.colorAttachments[0].blendingEnabled = YES;
  desc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
  desc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
  desc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
  desc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

  id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:desc
                                                                               error:&error];

  [vert_func release];
  [frag_func release];
  [vert_lib release];
  [frag_lib release];
  [desc release];

  if (error) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader_metal] Pipeline error: %s",
               [[error localizedDescription] UTF8String]);
    return nullptr;
  }

  return (void *)pipeline;
}

void *GPU_metal_pipeline_create_from_source(const char *source,
                                            const GPUShaderSourceParameters *params)
{
  auto device = (id<MTLDevice>)vpi::VPI_ContextMTL::get_current_device();
  if (!device) {
    CLOG_ERROR(LOG_SHADER, "[GPU_shader_metal] No Metal device available");
    return nullptr;
  }

  NSError *error = nil;
  MTLCompileOptions *options = [[MTLCompileOptions alloc] init];
  options.mathMode = MTLMathModeFast;

  id<MTLLibrary> library = [device newLibraryWithSource:[NSString stringWithUTF8String:source]
                                                options:options
                                                  error:&error];
  [options release];

  if (error) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader_metal] Source library error: %s",
               [[error localizedDescription] UTF8String]);
    return nullptr;
  }

  const char *v_entry = (params && params->vert_entry) ? params->vert_entry : "vertex_main";
  const char *f_entry = (params && params->frag_entry) ? params->frag_entry : "fragment_main";

  CLOG_INFO(LOG_SHADER, "[GPU_shader_metal] Creating functions: %s, %s", v_entry, f_entry);

  id<MTLFunction> vert_func = [library
      newFunctionWithName:[NSString stringWithUTF8String:v_entry]];
  id<MTLFunction> frag_func = [library
      newFunctionWithName:[NSString stringWithUTF8String:f_entry]];

  if (!vert_func || !frag_func) {
    CLOG_WARN(LOG_SHADER, "[GPU_shader_metal] Custom entry points failed, trying 'main'");
    if (!vert_func)
      vert_func = [library newFunctionWithName:@"main"];
    if (!frag_func)
      frag_func = [library newFunctionWithName:@"main"];

    if (!vert_func || !frag_func) {
      CLOG_ERROR(LOG_SHADER,
                 "[GPU_shader_metal] Failed to find entry points %s/%s or main",
                 v_entry,
                 f_entry);
      [library release];
      return nullptr;
    }
  }

  MTLRenderPipelineDescriptor *desc = [[MTLRenderPipelineDescriptor alloc] init];
  desc.vertexFunction = vert_func;
  desc.fragmentFunction = frag_func;
  desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
  desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
  desc.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

  // Editor defaults: Enable blending
  desc.colorAttachments[0].blendingEnabled = YES;
  desc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
  desc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
  desc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
  desc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

  CLOG_INFO(LOG_SHADER, "[GPU_shader_metal] Creating pipeline state...");
  id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:desc
                                                                               error:&error];
  [vert_func release];
  [frag_func release];
  [library release];
  [desc release];

  if (error) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader_metal] Pipeline error for source shader: %s",
               [[error localizedDescription] UTF8String]);
    return nullptr;
  }

  CLOG_INFO(LOG_SHADER, "[GPU_shader_metal] Successfully created Metal pipeline from source");
  return (void *)pipeline;
}

void GPU_metal_pipeline_free(void *pipeline)
{
  if (pipeline) {
    [(id<MTLRenderPipelineState>)pipeline release];
  }
}

void GPU_shader_bind_metal(GPUShader *shader, void *encoder)
{
  if (shader && shader->metal_pipeline && encoder) {
    auto mtl_encoder = (id<MTLRenderCommandEncoder>)encoder;
    [mtl_encoder setRenderPipelineState:(id<MTLRenderPipelineState>)shader->metal_pipeline];
  }
}

}  // namespace vektor::gpu
