// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Port__
#define __UI_GraphView_Port__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include <FTL/StrRef.h>

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

      char const *name() const
        { return m_name.c_str(); }
      void setName( char const *name );
      char const *path() const
        { return name(); }
      virtual char const * label() const;
      void setLabel(char const * n);
      virtual QColor color() const;
      void setColor(QColor color);
      virtual PortType portType() const;
      virtual char const * dataType() const;
      virtual void setDataType(char const * dt);
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);

      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const;
      
      virtual TargetType targetType() const { return TargetType_Port; }
      virtual QPointF connectionPos(PortType pType) const;

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
    };

  };

};

#endif // __UI_GraphView_Port__

