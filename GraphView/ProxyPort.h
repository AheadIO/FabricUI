// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_ProxyPort__
#define __UI_GraphView_ProxyPort__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include "PortType.h"
#include "PinCircle.h"
#include "ConnectionTarget.h"
#include "GraphicItemTypes.h"
#include "TextContainer.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward decl
    class Graph;
    class Pin;
    class SidePanel;

    class ProxyPort : public ConnectionTarget
    {
      Q_OBJECT

      friend class Graph;

    public:

      ProxyPort(SidePanel * parent, PortType portType);
      virtual ~ProxyPort() {}

      virtual int type() const { return QGraphicsItemType_ProxyPort; }

      SidePanel * sidePanel();
      const SidePanel * sidePanel() const;
      Graph * graph();
      const Graph * graph() const;

      virtual PortType portType() const;
      QColor color() const;
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);

      virtual bool canConnectTo(ConnectionTarget * other, QString &failureReason) const;
      virtual TargetType targetType() const { return TargetType_ProxyPort; }
      virtual QPointF connectionPos(PortType pType) const;

    private:

      SidePanel * m_sidePanel;
      PortType m_portType;
      bool m_highlighted;
      PinCircle * m_circle;
      TextContainer * m_label;
    };

  };

};

#endif // __UI_GraphView_ProxyPort__

