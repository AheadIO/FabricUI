// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneHub_SGLightManagerDialog__
#define __UI_SceneHub_SGLightManagerDialog__

#include <QtCore/QPoint>
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QFrame>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include "SGObjectManagerDialog.h"
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SGLightManagerDialog : public SGObjectManagerDialog
    {
      Q_OBJECT

      public:
        SGLightManagerDialog(QWidget* parent, FabricCore::Client *client, FabricCore::RTVal testObject);
        virtual ~SGLightManagerDialog() {};
        
        void setLightProperties(uint32_t lightType);
        void addLight(uint32_t lightType, QPoint screenPos);

      public slots:
        void showColorDialog();
        void setLightProperties();
        void updateLightProperties();
 
      private:
        bool m_addLight;
        uint32_t m_lightType;
        
        QPoint m_glWidgetScreenPos;
        QPushButton *m_colorButton;
        QPushButton *m_addButton;
        QSlider *m_intensitySlider;
    };
  };
};

#endif // __UI_SceneHub_SGLightManagerDialog__
