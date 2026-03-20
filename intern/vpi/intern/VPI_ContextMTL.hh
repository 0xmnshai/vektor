#pragma once

#include "../VPI_Types.h"
#include "VPI_Context.hh"
#include "VPI_Window.hh"

#ifdef __OBJC__
#  include <AppKit/AppKit.h>
#  include <Cocoa/Cocoa.h>
#  include <Metal/Metal.h>
#  include <QuartzCore/QuartzCore.h>
#else

typedef struct objc_object MTLDevice;
typedef struct objc_object MTLCommandQueue;
typedef struct objc_object NSView;
typedef struct objc_object CAMetalLayer;
typedef struct objc_object MTLRenderPassDescriptor;
typedef struct objc_object MTLRenderPipelineState;
typedef struct objc_object MTLTexture;
typedef struct objc_object CAMetalDrawable;

#endif /* __OBJC__ */

#define MTLDeviceRef void *
#define MTLCommandQueueRef void *
#define MTLRenderPipelineStateRef void *
#define MTLTextureRef void *
#define CAMetalDrawableRef void *

namespace vpi {
class VPI_ContextMTL : public VPI_Context {
 public:
  explicit VPI_ContextMTL(const VPI_ContextParams &context_params,
                          VPI_Window *window,
                          void *metalView,
                          void *metalLayer);

  ~VPI_ContextMTL() override;

  [[nodiscard]] VPI_TSuccess init_context() const override;

  [[nodiscard]] VPI_TSuccess release_context() const override;

  [[nodiscard]] VPI_TSuccess release_native_handles() const override;

  void resize_context(uint32_t width, uint32_t height) const override;

  static MTLDeviceRef get_current_device();

  [[nodiscard]] void *get_current_command_encoder() const
  {
    return current_encoder_;
  }
  [[nodiscard]] void *get_current_command_buffer() const
  {
    return current_command_buffer_;
  }
  [[nodiscard]] void *get_current_render_pass_descriptor() const
  {
    return current_pass_desc_;
  }

  void begin_render_pass();
  void end_render_pass();

  void set_depth_write_enabled(bool enabled);

#ifdef __OBJC__
  static id<MTLDevice> get_shared_device();
  static id<MTLCommandQueue> get_shared_command_queue();
#else
  static void *get_shared_device();
  static void *get_shared_command_queue();
#endif

#ifdef __OBJC__
  void metal_register_present_callbacks(void (*callback)(
      MTLRenderPassDescriptor *, id<MTLRenderPipelineState>, id<MTLTexture>, id<CAMetalDrawable>));

  id<MTLDevice> get_metal_device() const
  {
    return metal_device_;
  }
  id<MTLCommandQueue> get_metal_command_queue() const
  {
    return metal_command_queue_;
  }
#else
  void metal_register_present_callbacks(void (*callback)(
      MTLRenderPassDescriptor *, MTLRenderPipelineStateRef, MTLTextureRef, CAMetalDrawableRef));

  [[nodiscard]] MTLDeviceRef get_metal_device() const
  {
    return metal_device_;
  }
  [[nodiscard]] MTLCommandQueueRef get_metal_command_queue() const
  {
    return metal_command_queue_;
  }
#endif

 protected:
  static int s_shared_count_;
#ifdef __OBJC__
  static id<MTLDevice> s_shared_device_;
  static id<MTLCommandQueue> s_shared_queue_;
#else
  static void *s_shared_device_;
  static void *s_shared_queue_;
#endif

  NSView *metal_view_;
  NSView *metal_subview_;
  CAMetalLayer *metal_layer_;
#ifdef __OBJC__
  id<MTLDevice> metal_device_;
  id<MTLCommandQueue> metal_command_queue_;
  id<MTLRenderPipelineState> metal_render_pipeline_;
#else
  MTLDeviceRef metal_device_;
  MTLCommandQueueRef metal_command_queue_;
  MTLRenderPipelineStateRef metal_render_pipeline_;
#endif
  bool owns_metal_device_;

  static const int METAL_SWAPCHAIN_SIZE = 3;
  struct MTLSwapchainTexture {
#ifdef __OBJC__
    id<MTLTexture> texture;
#else
    MTLTextureRef texture;
#endif
    unsigned int index;
  };
  MTLSwapchainTexture default_framebuffer_metal_texture_[METAL_SWAPCHAIN_SIZE];
  unsigned int current_swapchain_index = 0;

  unsigned int last_width_ = 0;
  unsigned int last_height_ = 0;

#ifdef __OBJC__
  id<MTLTexture> depth_texture_;
  id<MTLDepthStencilState> depth_state_read_write_;
  id<MTLDepthStencilState> depth_state_read_only_;
#else
  MTLTextureRef depth_texture_;
  void *depth_state_read_write_;
  void *depth_state_read_only_;
#endif

#ifdef __OBJC__
  void (*contextPresentCallback)(MTLRenderPassDescriptor *,
                                 id<MTLRenderPipelineState>,
                                 id<MTLTexture>,
                                 id<CAMetalDrawable>);
  dispatch_semaphore_t frame_semaphore_;
#else
  void (*contextPresentCallback)(MTLRenderPassDescriptor *,
                                 MTLRenderPipelineStateRef,
                                 MTLTextureRef,
                                 CAMetalDrawableRef);
  void *frame_semaphore_;
#endif

  int mtl_SwapInterval;

  /* Metal functions */
  void metal_init();
  void metal_free();
  void metal_init_framebuffers();
  void metal_update_Framebuffer();
  void metal_init_clear() {};

  void *current_command_buffer_ = nullptr;
  void *current_encoder_ = nullptr;
  void *current_pass_desc_ = nullptr;
};
}  // namespace vpi
