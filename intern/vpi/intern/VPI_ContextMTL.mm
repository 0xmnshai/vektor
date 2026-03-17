#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#include <cassert>
#include <iostream>
#include <mutex>
#include <vector>

#include "VPI_ContextMTL.hh"

static id<MTLDevice> g_current_metal_device = nil;

namespace vpi {

id<MTLDevice> VPI_ContextMTL::s_shared_device_ = nil;
id<MTLCommandQueue> VPI_ContextMTL::s_shared_queue_ = nil;
int VPI_ContextMTL::s_shared_count_ = 0;

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
      last_width_(0),
      last_height_(0),
      contextPresentCallback(nullptr),
      mtl_SwapInterval(0),
      frame_semaphore_(nil),
      depth_texture_(nil),
      depth_state_read_write_(nil),
      depth_state_read_only_(nil)
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

  if (s_shared_device_ == nil) {
    s_shared_device_ = MTLCreateSystemDefaultDevice();
    s_shared_queue_ = [s_shared_device_ newCommandQueue];
  }
  s_shared_count_++;

  metal_device_ = s_shared_device_;
  metal_command_queue_ = s_shared_queue_;
  g_current_metal_device = metal_device_;

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
      metal_layer_.displaySyncEnabled = NO;

      [metal_subview_ setWantsLayer:YES];
      [metal_subview_ setLayer:metal_layer_];
      [metal_view_ addSubview:metal_subview_];
    }
  }

  frame_semaphore_ = dispatch_semaphore_create(3);

  MTLDepthStencilDescriptor *depthDesc = [[MTLDepthStencilDescriptor alloc] init];
  depthDesc.depthCompareFunction = MTLCompareFunctionLess;
  depthDesc.depthWriteEnabled = YES;
  depth_state_read_write_ = [metal_device_ newDepthStencilStateWithDescriptor:depthDesc];

  depthDesc.depthWriteEnabled = NO;
  depth_state_read_only_ = [metal_device_ newDepthStencilStateWithDescriptor:depthDesc];
  [depthDesc release];

  metal_update_Framebuffer();
}

void VPI_ContextMTL::metal_free()
{
  s_shared_count_--;
  if (s_shared_count_ <= 0) {
    [s_shared_queue_ release];
    [s_shared_device_ release];
    s_shared_queue_ = nil;
    s_shared_device_ = nil;
  }

  if (metal_subview_) {
    [metal_subview_ removeFromSuperview];
    [metal_subview_ release];
    metal_subview_ = nil;
  }

  if (depth_texture_) {
    [depth_texture_ release];
    depth_texture_ = nil;
  }

  if (depth_state_read_write_) {
    [depth_state_read_write_ release];
    depth_state_read_write_ = nil;
  }

  if (depth_state_read_only_) {
    [depth_state_read_only_ release];
    depth_state_read_only_ = nil;
  }

  metal_command_queue_ = nil;
  metal_device_ = nil;
  metal_layer_ = nil;
  metal_view_ = nil;
}

void VPI_ContextMTL::metal_init_framebuffers() {}

void VPI_ContextMTL::metal_update_Framebuffer()
{
  if (metal_layer_ && metal_subview_) {
    CGFloat scale = [NSScreen mainScreen].backingScaleFactor;
    CGSize size = metal_subview_.bounds.size;
    uint32_t width = (uint32_t)(size.width * scale);
    uint32_t height = (uint32_t)(size.height * scale);

    if (width == 0 || height == 0)
      return;
    if (width == last_width_ && height == last_height_ && depth_texture_)
      return;

    metal_layer_.drawableSize = CGSizeMake(width, height);
    if (metal_layer_.contentsScale != scale) {
      metal_layer_.contentsScale = scale;
    }

    if (depth_texture_) {
      [depth_texture_ release];
    }

    MTLTextureDescriptor *depthTexDesc = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8
                                     width:width
                                    height:height
                                 mipmapped:NO];
    depthTexDesc.usage = MTLTextureUsageRenderTarget;
    depthTexDesc.storageMode = MTLStorageModePrivate;
    depth_texture_ = [metal_device_ newTextureWithDescriptor:depthTexDesc];

    last_width_ = width;
    last_height_ = height;
  }
}

void VPI_ContextMTL::begin_render_pass()
{
  s_active_context_ = this;
  if (!metal_layer_) {
    return;
  }

  // Ensure framebuffer and scale are up to date before grabbing a drawable (fixes startup/resize
  // blur)
  metal_update_Framebuffer();

  // Use a very short timeout to skip frames if GPU is saturated, avoiding UI lockup
  if (dispatch_semaphore_wait(frame_semaphore_,
                              dispatch_time(DISPATCH_TIME_NOW, 10 * NSEC_PER_MSEC)) != 0)
  {
    return;
  }

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

  if (depth_texture_) {
    passDescriptor.depthAttachment.texture = depth_texture_;
    passDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
    passDescriptor.depthAttachment.storeAction = MTLStoreActionDontCare;
    passDescriptor.depthAttachment.clearDepth = 1.0;
  }

  id<MTLRenderCommandEncoder> encoder = [commandBuffer
      renderCommandEncoderWithDescriptor:passDescriptor];

  [encoder setDepthStencilState:depth_state_read_write_];
  [encoder setCullMode:MTLCullModeNone];

  MTLViewport viewport = {
      0.0, 0.0, (double)drawable.texture.width, (double)drawable.texture.height, 0.0, 1.0};
  [encoder setViewport:viewport];
  MTLScissorRect scissor = {
      0, 0, (NSUInteger)drawable.texture.width, (NSUInteger)drawable.texture.height};
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
  auto drawable = (id<CAMetalDrawable>)user_data_;

  [encoder endEncoding];

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

void VPI_ContextMTL::set_depth_write_enabled(bool enabled)
{
  if (!current_encoder_) {
    return;
  }
  auto encoder = (id<MTLRenderCommandEncoder>)current_encoder_;
  [encoder setDepthStencilState:enabled ? depth_state_read_write_ : depth_state_read_only_];
}

id<MTLDevice> VPI_ContextMTL::get_shared_device()
{
  return s_shared_device_;
}
id<MTLCommandQueue> VPI_ContextMTL::get_shared_command_queue()
{
  return s_shared_queue_;
}

}  // namespace vpi

MTLDeviceRef vpi::VPI_ContextMTL::get_current_device()
{
  return (MTLDeviceRef)vpi::VPI_ContextMTL::get_shared_device();
}
