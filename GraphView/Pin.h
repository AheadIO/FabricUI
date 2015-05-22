// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Pin__
#define __UI_GraphView_Pin__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include "PortType.h"
#include "PinLabel.h"
#include "PinCircle.h"
#include "ConnectionTarget.h"
#include "GraphicItemTypes.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward decl
    class Graph;
    class Node;

    class Pin : public ConnectionTarget
    {
      Q_OBJECT

    public:

      virtual ~Pin() {}

      virtual int type() const { return QGraphicsItemType_Pin; }

      Node * node();
      const Node * node() const;
      Graph * graph();
      const Graph * graph() const;
      virtual QString name() const;
      virtual QString path() const;
      virtual QString label() const;
      virtual PortType portType() const;
      QColor color() const;
      void setColor(QColor color, bool quiet = false);
      virtual int index() const;
      virtual void setIndex(int i);
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);
      virtual QString dataType() const;
      virtual void setDataType(QString type);

      PinCircle * inCircle();
      const PinCircle * inCircle() const;
      PinCircle * outCircle();
      const PinCircle * outCircle() const;

      virtual bool canConnectTo(ConnectionTarget * other, QString &failureReason) const;
      virtual TargetType targetType() const { return TargetType_Pin; }
      virtual QPointF connectionPos(PortType pType) const;

      virtual void setDrawState(bool flag);
      virtual bool drawState() const;

      // accessed by controller
      Pin(Node * parent, QString name, PortType pType, QColor color, QString label = "");
      
    signals:

      void colorChanged(Pin*, QColor);

    private:

      Node * m_node;
      QString m_name;
      PortType m_portType;
      QString m_dataType;
      QString m_labelCaption;
      bool m_highlighted;
      QColor m_color;
      int m_index;
      PinLabel * m_label;
      PinCircle * m_inCircle;
      PinCircle * m_outCircle;
      bool m_drawState;
    };

  };

};

#endif // __UI_GraphView_Pin__

