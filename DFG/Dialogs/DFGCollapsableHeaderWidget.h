// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGCollapsableHeaderWidget__
#define __UI_DFG_DFGCollapsableHeaderWidget__

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include "../DFGConfig.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGCollapsableHeaderWidget : public QLabel
    {
      Q_OBJECT

    public:

      DFGCollapsableHeaderWidget(QWidget * parent, QString title, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGCollapsableHeaderWidget();

      virtual void mousePressEvent(QMouseEvent * event);

    signals:

      void toggled();

    private:

      DFGConfig m_dfgConfig;
    };

  };

};

#endif // __UI_DFG_DFGCollapsableHeaderWidget__
