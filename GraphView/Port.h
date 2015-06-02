// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Port__
#define __UI_GraphView_Port__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>

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
        char const *name,
        PortType portType,
        QString dataType,
        QColor color,
        QString label = ""
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
      virtual QString label() const;
      void setLabel(QString n);
      virtual QColor color() const;
      void setColor(QColor color);
      virtual PortType portType() const;
      virtual QString dataType() const;
      virtual void setDataType(QString dt);
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
      QString m_labelCaption;
      QColor m_color;
      QString m_dataType;
      bool m_highlighted;
      TextContainer * m_label;
      PinCircle * m_circle;
    };

  };

};

#endif // __UI_GraphView_Port__

