// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Pin__
#define __UI_GraphView_Pin__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include <FTL/CStrRef.h>

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

      Node *node()
        { return m_node; }
      const Node * node() const
        { return m_node; }
      Graph * graph();
      const Graph * graph() const;

      FTL::CStrRef name() const
        { return m_name; }
      void setName( FTL::StrRef newName );

      virtual std::string path() const;

      virtual char const * label() const;
      PinLabel * labelWidget() { return m_label; }
      virtual PortType portType() const;
      QColor color() const;
      void setColor(QColor color, bool quiet = false, bool performUpdate = true);
      virtual int index() const;
      virtual void setIndex(int i);
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);
      virtual FTL::CStrRef dataType() const
        { return m_dataType; }
      virtual void setDataType(FTL::CStrRef dataType);

      PinCircle * inCircle();
      const PinCircle * inCircle() const;
      PinCircle * outCircle();
      const PinCircle * outCircle() const;

      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const;

      virtual TargetType targetType() const { return TargetType_Pin; }
      virtual QPointF connectionPos(PortType pType) const;

      virtual void setDrawState(bool flag);
      virtual bool drawState() const;
      virtual void setDaisyChainCircleVisible(bool flag);

      // accessed by controller
      Pin(
        Node * parent,
        FTL::StrRef name,
        PortType pType,
        QColor color,
        FTL::StrRef label = FTL::StrRef()
        );
      
    signals:

      void colorChanged(Pin*, QColor);

      void inCircleScenePositionChanged();
      void outCircleScenePositionChanged();

    private:

      Node * m_node;
      std::string m_name;
      PortType m_portType;
      std::string m_dataType;
      std::string m_labelCaption;
      std::string m_labelSuffix;
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

