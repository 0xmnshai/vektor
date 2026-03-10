#pragma once

#include "VPI_Types.h"
#include "intern/VPI_Context.hh"
#include "intern/VPI_Window.hh"

#ifdef __OBJC__
#  include <AppKit/AppKit.h>
#  include <Cocoa/Cocoa.h>
#  include <Metal/Metal.h>
#  include <QuartzCore/QuartzCore.h>
#else

typedef struct objc_object NSView;
typedef struct objc_object CAMetalLayer;
typedef struct objc_object MTLRenderPassDescriptor;
typedef struct objc_object MTLRenderPipelineState;
typedef struct objc_object MTLTexture;
typedef struct objc_object CAMetalDrawable;

#  define MTLRenderPipelineStateRef void *
#  define MTLTextureRef void *
#  define CAMetalDrawableRef void *
#endif /* __OBJC__ */

namespace vpi {
class VPI_ContextMTL : public VPI_Context {
  explicit VPI_ContextMTL(const VPI_ContextParams &context_params,
                          NSView *metalView,
                          CAMetalLayer *metalLayer);

  ~VPI_ContextMTL() override;

  [[nodiscard]] VPI_TSuccess activate_context() const override = 0;

  [[nodiscard]] VPI_TSuccess release_context() const override = 0;

  [[nodiscard]] virtual VPI_TSuccess initialise_context() const override = 0;

  virtual VPI_TSuccess release_native_handles() override = 0;

#ifdef __OBJC__
  void metal_register_present_callbacks(void (*callback)(
      MTLRenderPassDescriptor *, id<MTLRenderPipelineState>, id<MTLTexture>, id<CAMetalDrawable>));
#else
  void metal_register_present_callbacks(void (*callback)(
      MTLRenderPassDescriptor *, MTLRenderPipelineStateRef, MTLTextureRef, CAMetalDrawableRef));
#endif

 protected:
  VPI_Window *window_;
  static int s_shared_count_;

  NSView *metal_view_;
  CAMetalLayer *metal_layer_;
#ifdef __OBJC__
  id<MTLRenderPipelineState> metal_render_pipeline_;
#else
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

#ifdef __OBJC__
  void (*contextPresentCallback)(MTLRenderPassDescriptor *,
                                 id<MTLRenderPipelineState>,
                                 id<MTLTexture>,
                                 id<CAMetalDrawable>);
#else
  void (*contextPresentCallback)(MTLRenderPassDescriptor *,
                                 MTLRenderPipelineStateRef,
                                 MTLTextureRef,
                                 CAMetalDrawableRef);
#endif

  int mtl_SwapInterval;

  /* Metal functions */
  void metal_init();
  void metal_free();
  void metal_init_framebuffers();
  void metal_update_Framebuffer();
  void metal_init_clear() {};
};
}  // namespace vpi
