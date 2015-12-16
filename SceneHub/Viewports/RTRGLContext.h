/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */


#ifndef _H_RTRGLContext_
#define _H_RTRGLContext_

#include <QtOpenGL/QGLWidget>

namespace FabricUI
{
  namespace Viewports
  {
    #ifdef __APPLE__
    void *TryFormat(
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
      return TryFormat(
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

#endif // _H_RTRGLContext_
