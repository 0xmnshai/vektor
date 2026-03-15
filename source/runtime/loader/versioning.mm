#include "versioning.h"
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLFunctions_4_1_Core>

#import <Metal/Metal.h>

const char *VKT_version_opengl_get(void)
{
  QOpenGLContext *context = QOpenGLContext::currentContext();
  if (context) {
    QOpenGLFunctions *f = context->functions();
    return (const char *)f->glGetString(GL_VERSION);
  }
  return "Unknown";
}

const char *VKT_version_metal_get(void)
{
  @autoreleasepool {
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (device) {
      NSString *name = [device name];
      return [name UTF8String];
    }
  }
  return "Unknown";
}
