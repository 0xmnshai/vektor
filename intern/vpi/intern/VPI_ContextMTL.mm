#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include <cassert>
#include <mutex>
#include <vector>

#include "VPI_ContextMTL.hh"

namespace vpi {
VPI_ContextMTL::VPI_ContextMTL(const VPI_ContextParams &context_params,
                               void *metalView,
                               void *metalLayer)
    : VPI_Context(context_params),
      window_(nullptr),
      metal_view_((NSView *)metalView),
      metal_subview_(nullptr),
      metal_layer_((CAMetalLayer *)metalLayer),
      metal_device_(nil),
      metal_command_queue_(nil),
      metal_render_pipeline_(nil),
      owns_metal_device_(false),
      default_framebuffer_metal_texture_{},
      current_swapchain_index(0),
      contextPresentCallback(nullptr),
      mtl_SwapInterval(0)
{
}

VPI_ContextMTL::~VPI_ContextMTL()
{
  metal_free();
}

VPI_TSuccess VPI_ContextMTL::init_context() const
{
  const_cast<VPI_ContextMTL *>(this)->metal_init();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_ContextMTL::release_context() const
{
  const_cast<VPI_ContextMTL *>(this)->metal_free();
  return VPI_kSuccess;
}

VPI_TSuccess VPI_ContextMTL::release_native_handles() const
{
  return VPI_kSuccess;
}
 
void VPI_ContextMTL::resize_context(uint32_t /*width*/, uint32_t /*height*/) const
{
  const_cast<VPI_ContextMTL *>(this)->metal_update_Framebuffer();
}

void VPI_ContextMTL::metal_init()
{
  if (metal_device_) {
    return;
  }

  metal_device_ = MTLCreateSystemDefaultDevice();
  if (!metal_device_) {
    return;
  }

  metal_command_queue_ = [metal_device_ newCommandQueue];

  if (metal_view_) {
    if (!metal_layer_) {
      metal_subview_ = [[NSView alloc] initWithFrame:metal_view_.bounds];
      [metal_subview_ setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
  
      metal_layer_ = [CAMetalLayer layer];
      metal_layer_.device = metal_device_;
      metal_layer_.pixelFormat = MTLPixelFormatBGRA8Unorm;
      metal_layer_.framebufferOnly = YES;
 
      [metal_subview_ setWantsLayer:YES];
      [metal_subview_ setLayer:metal_layer_];
      [metal_view_ addSubview:metal_subview_];
    }
  }

  metal_init_framebuffers();
}

void VPI_ContextMTL::metal_free()
{
  if (metal_subview_) {
    [metal_subview_ removeFromSuperview];
    metal_subview_ = nil;
  }
  metal_command_queue_ = nil;
  metal_device_ = nil;
  metal_layer_ = nil;
  metal_view_ = nil;
}

void VPI_ContextMTL::metal_init_framebuffers()
{
  // Initialization of swapchain textures if needed
}

void VPI_ContextMTL::metal_update_Framebuffer()
{
  if (metal_layer_ && metal_subview_) {
    CGSize size = metal_subview_.bounds.size;
    metal_layer_.drawableSize = size;
  }
}

void VPI_ContextMTL::metal_register_present_callbacks(void (*callback)(
    MTLRenderPassDescriptor *, id<MTLRenderPipelineState>, id<MTLTexture>, id<CAMetalDrawable>))
{
  contextPresentCallback = callback;
}
}  // namespace vpi
