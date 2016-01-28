/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef _UI_SCENEHUB_RTRGLCONTEXT_H__
#define _UI_SCENEHUB_RTRGLCONTEXT_H__

#include <QtOpenGL/QGLWidget>

namespace FabricUI
{
  namespace Viewports
  {
#ifdef __APPLE__
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
      );

    inline void *TryFormat( QGLFormat const &qglFormat )
    {
      return TryFormatImpl(
        qglFormat.doubleBuffer(),
        qglFormat.stereo(),
        qglFormat.alpha(),
        qglFormat.alphaBufferSize(),
        qglFormat.stencil(),
        qglFormat.stencilBufferSize(),
        qglFormat.depth(),
        qglFormat.depthBufferSize(),
        qglFormat.accum(),
        qglFormat.accumBufferSize(),
        qglFormat.sampleBuffers(),
        qglFormat.samples()
        );
    }
#endif

    class RTRGLContext : public QGLContext
    {
      QGLFormat m_qglFormat;

    public:

      RTRGLContext( QGLFormat const &qglFormat )
        : QGLContext( qglFormat )
        , m_qglFormat( qglFormat )
      {
      }

    #ifdef __APPLE__
      virtual void *chooseMacVisual(GDHandle /* handle */)
      {
        void *fmt = TryFormat(m_qglFormat);
        if (!fmt && m_qglFormat.stereo()) {
            m_qglFormat.setStereo(false);
            fmt = TryFormat(m_qglFormat);
        }
        if (!fmt && m_qglFormat.sampleBuffers()) {
            m_qglFormat.setSampleBuffers(false);
            fmt = TryFormat(m_qglFormat);
        }
        if (!fmt)
            qWarning("QGLContext::chooseMacVisual: Unable to choose a pixel format");
        return fmt;
      }
    #endif
    };
  };
};

#endif // _UI_SCENEHUB_RTRGLCONTEXT_H__
