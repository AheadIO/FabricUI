/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifdef __APPLE__

#import <AppKit/AppKit.h>
#include "RTRGLContext.h"

using namespace FabricUI::Viewports;

static inline int def(int val, int defVal)
{
  return val != -1 ? val : defVal;
}

extern "C" void *TryFormatImpl(
  bool doubleBuffer,
  bool stereo,
  bool alpha,
  int alphaBufferSize,
  bool stencil,
  int stencilBufferSize,
  bool depth,
  int depthBufferSize,
  bool accum,
  int accumBufferSize,
  bool sampleBuffers,
  int samples
  )
{
  static const int Max = 40;
  NSOpenGLPixelFormatAttribute attribs[Max];
  int cnt = 0;

  attribs[cnt++] = NSOpenGLPFAOpenGLProfile;
  attribs[cnt++] = (NSOpenGLPixelFormatAttribute)NSOpenGLProfileVersion3_2Core;

  attribs[cnt++] = NSOpenGLPFAColorSize;
  attribs[cnt++] = 32;

  if (doubleBuffer)
      attribs[cnt++] = NSOpenGLPFADoubleBuffer;
  if (stereo)
      attribs[cnt++] = NSOpenGLPFAStereo;
  if (alpha) {
      attribs[cnt++] = NSOpenGLPFAAlphaSize;
      attribs[cnt++] = def(alphaBufferSize, 8);
  }
  if (stencil) {
      attribs[cnt++] = NSOpenGLPFAStencilSize;
      attribs[cnt++] = def(stencilBufferSize, 8);
  }
  if (depth) {
      attribs[cnt++] = NSOpenGLPFADepthSize;
      attribs[cnt++] = def(depthBufferSize, 32);
  }
  if (accum) {
      attribs[cnt++] = NSOpenGLPFAAccumSize;
      attribs[cnt++] = def(accumBufferSize, 1);
  }
  if (sampleBuffers) {
      attribs[cnt++] = NSOpenGLPFASampleBuffers;
      attribs[cnt++] = 1; 
      attribs[cnt++] = NSOpenGLPFASamples;
      attribs[cnt++] = def(samples, 4);
  }

  attribs[cnt] = 0;
  return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
}

#endif
