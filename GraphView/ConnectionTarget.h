// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_ConnectionTarget__
#define __UI_GraphView_ConnectionTarget__

#include <QtGui/QGraphicsWidget>
#include <QtCore/QPointF>
#include "PortType.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;

    enum TargetType
    {
      TargetType_Pin,
      TargetType_Port,
      TargetType_ProxyPort,
      TargetType_MouseGrabber
    };

    class ConnectionTarget : public QGraphicsWidget
    {

    public:

      ConnectionTarget(QGraphicsWidget * parent) : QGraphicsWidget(parent) {}

      virtual bool canConnectTo(ConnectionTarget * other, QString &failureReason) const = 0;
      virtual TargetType targetType() const = 0;
      virtual QPointF connectionPos(PortType pType) const = 0;
    
      virtual Graph * graph() = 0;
      virtual const Graph * graph() const = 0;

      virtual bool highlighted() const = 0;
      virtual void setHighlighted(bool state = true) = 0;

      virtual bool isConnected() const;
    };

  };

};

#endif // __UI_GraphView_ConnectionTarget__
