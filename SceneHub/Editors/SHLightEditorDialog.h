/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHLightEditorDialog_H__
#define __UI_SCENEHUB_SHLightEditorDialog_H__

#include <QtCore/QPoint>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include "SHBaseEditorDialog.h"

namespace FabricUI
{
  namespace SceneHub
  {
    class SHLightEditorDialog : public SHBaseEditorDialog
    {
      Q_OBJECT

      public:
        SHLightEditorDialog(
          QWidget*parent, 
          SHGLScene *shGLScene, 
          SHGLRenderer *shGLRenderer);

        virtual ~SHLightEditorDialog() {};

      public slots:
        virtual void showColorDialog();
        
        void updateLightProperties();
 
      private:
        QPoint m_glWidgetScreenPos;
        QPushButton *m_colorButton;
        QPushButton *m_closeButton;
        QSlider *m_intensitySlider;
    };
  }
}

#endif // __UI_SCENEHUB_SHLightEditorDialog_H__
