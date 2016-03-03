// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Port__
#define __UI_GraphView_Port__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <FTL/CStrRef.h>

#include "PortType.h"
#include "TextContainer.h"
#include "PinCircle.h"
#include "ConnectionTarget.h"
#include "GraphicItemTypes.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward decl
    class Graph;
    class Pin;
    class SidePanel;

    class Port : public ConnectionTarget
    {
      Q_OBJECT

      friend class Graph;
      friend class SidePanel;

    public:

      Port(
        SidePanel * parent,
        FTL::StrRef name,
        PortType portType,
        FTL::StrRef dataType,
        QColor color,
        FTL::StrRef label = FTL::StrRef()
        );
      virtual ~Port() {}

      virtual int type() const { return QGraphicsItemType_Port; }

      SidePanel * sidePanel();
      const SidePanel * sidePanel() const;
      Graph * graph();
      const Graph * graph() const;
      PinCircle * circle();
      const PinCircle * circle() const;

      FTL::CStrRef name() const
        { return m_name; }
      void setName( FTL::CStrRef name );

      virtual std::string path() const;

      virtual char const * label() const;
      void setLabel(char const * n);
      virtual QColor color() const;
      void setColor(QColor color);
      virtual PortType portType() const;
      unsigned int index() const { return m_index; }

      virtual FTL::CStrRef dataType() const
        { return m_dataType; }
      virtual void setDataType(FTL::CStrRef dataType);

      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);

      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const;
      
      virtual TargetType targetType() const { return TargetType_Port; }
      virtual QPointF connectionPos(PortType pType) const;

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

    signals:

      void positionChanged();

    protected:

      void setIndex(unsigned id) { m_index = id; }

    private:

      void init();

      SidePanel * m_sidePanel;
      std::string m_name;
      PortType m_portType;
      std::string m_labelCaption;
      QColor m_color;
      std::string m_dataType;
      bool m_highlighted;
      TextContainer * m_label;
      PinCircle * m_circle;
      unsigned int m_index;
    };

  };

};

#endif // __UI_GraphView_Port__

