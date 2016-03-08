/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHGeometryEditorDialog_H__
#define __UI_SCENEHUB_SHGeometryEditorDialog_H__

#include <QtGui/QPushButton>
#include "SHBaseEditorDialog.h"
  
namespace FabricUI
{
  namespace SceneHub
  {
    class SHGeometryEditorDialog : public SHBaseEditorDialog
    {
      Q_OBJECT

      public:
        SHGeometryEditorDialog(QWidget*parent, SHGLScene *shGLScene);

        virtual ~SHGeometryEditorDialog() {};
 
      public slots:
        virtual void showColorDialog();

        void updatetGeometryProperties();

      private:       
        QPoint m_screenPos;
        QPushButton *m_colorButton;
        QPushButton *m_closeButton;

    };
  }
}

#endif // __UI_SCENEHUB_SHGeometryEditorDialog_H__
