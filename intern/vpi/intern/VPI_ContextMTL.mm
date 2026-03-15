#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#include <iostream>

#include <cassert>
#include <mutex>
#include <vector>

#include "VPI_ContextMTL.hh"

static id<MTLDevice> g_current_metal_device = nil;

namespace vpi {

VPI_ContextMTL::VPI_ContextMTL(const VPI_ContextParams &context_params,
                               vpi::VPI_Window *window,
                               void *metalView,
                               void *metalLayer)
    : VPI_Context(context_params, window),
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
      mtl_SwapInterval(0),
      frame_semaphore_(nil)
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
  g_current_metal_device = metal_device_;

  metal_command_queue_ = [metal_device_ newCommandQueue];

  if (metal_view_) {
    if (!metal_layer_) {
      metal_subview_ = [[NSView alloc] initWithFrame:metal_view_.bounds];
      [metal_subview_ setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

      metal_layer_ = [CAMetalLayer layer];
      metal_layer_.device = metal_device_;
      metal_layer_.pixelFormat = MTLPixelFormatBGRA8Unorm;
      metal_layer_.framebufferOnly = YES;
      metal_layer_.contentsScale = [NSScreen mainScreen].backingScaleFactor;
      metal_layer_.presentsWithTransaction = NO;
      metal_layer_.maximumDrawableCount = 3;
      metal_layer_.opaque = YES;
      metal_layer_.displaySyncEnabled = YES;

      [metal_subview_ setWantsLayer:YES];
      [metal_subview_ setLayer:metal_layer_];
      [metal_view_ addSubview:metal_subview_];
    }
  }

  frame_semaphore_ = dispatch_semaphore_create(METAL_SWAPCHAIN_SIZE);
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
    CGFloat scale = [NSScreen mainScreen].backingScaleFactor;
    CGSize size = metal_subview_.bounds.size;
    metal_layer_.drawableSize = CGSizeMake(size.width * scale, size.height * scale);
    metal_layer_.contentsScale = scale;
  }
}

void VPI_ContextMTL::metal_register_present_callbacks(void (*callback)(
    MTLRenderPassDescriptor *, id<MTLRenderPipelineState>, id<MTLTexture>, id<CAMetalDrawable>))
{
  contextPresentCallback = callback;
}
void VPI_ContextMTL::begin_render_pass()
{
  s_active_context_ = this;
  if (!metal_layer_) {
    return;
  }

  dispatch_semaphore_wait(frame_semaphore_, DISPATCH_TIME_FOREVER);
  id<CAMetalDrawable> drawable = [metal_layer_ nextDrawable];
  if (!drawable) {
    dispatch_semaphore_signal(frame_semaphore_);
    return;
  }

  id<MTLCommandBuffer> commandBuffer = [metal_command_queue_ commandBuffer];
  MTLRenderPassDescriptor *passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
  passDescriptor.colorAttachments[0].texture = drawable.texture;
  passDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
  passDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  passDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.15, 0.15, 0.15, 1.0);

  id<MTLRenderCommandEncoder> encoder = [commandBuffer
      renderCommandEncoderWithDescriptor:passDescriptor];

  [encoder setCullMode:MTLCullModeNone];

  // Set viewport and scissor
  MTLViewport viewport = {
      0.0, 0.0, (double)drawable.texture.width, (double)drawable.texture.height, 0.0, 1.0};
  [encoder setViewport:viewport];
  MTLScissorRect scissor = {0, 0, drawable.texture.width, drawable.texture.height};
  [encoder setScissorRect:scissor];

  current_command_buffer_ = (void *)commandBuffer;
  current_pass_desc_ = (void *)passDescriptor;
  current_encoder_ = (void *)encoder;

  user_data_ = (void *)drawable;
}

void VPI_ContextMTL::end_render_pass()
{
  if (!current_encoder_) {
    return;
  }

  auto encoder = (id<MTLRenderCommandEncoder>)current_encoder_;
  auto commandBuffer = (id<MTLCommandBuffer>)current_command_buffer_;

  [encoder endEncoding];

  auto drawable = (id<CAMetalDrawable>)user_data_;

  [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
    dispatch_semaphore_signal(frame_semaphore_);
  }];

  [commandBuffer presentDrawable:drawable];
  [commandBuffer commit];

  current_encoder_ = nullptr;
  current_command_buffer_ = nullptr;
  current_pass_desc_ = nullptr;
  user_data_ = nullptr;
}

}  // namespace vpi

MTLDeviceRef vpi::VPI_ContextMTL::get_current_device()
{
  return (MTLDeviceRef)g_current_metal_device;
}
