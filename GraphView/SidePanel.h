// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_SidePanel__
#define __UI_GraphView_SidePanel__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsLinearLayout>

#include "PortType.h"
#include "Port.h"
#include "ProxyPort.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;

    class SidePanel : public QGraphicsWidget
    {
      Q_OBJECT

      friend class Graph;

    public:

      SidePanel(Graph * parent, PortType portType, QColor color = QColor());

      Graph * graph();
      const Graph * graph() const;

      QString path() const;
      QString name() const;
      QColor color() const;
      PortType portType() const;
      
      virtual unsigned int portCount() const;
      virtual Port * port(unsigned int index);
      virtual Port * port(FTL::StrRef name);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      // accessed through controller
      virtual Port * addPort(Port * port);
      virtual bool removePort(Port * port);

    signals:
      void doubleClicked(FabricUI::GraphView::SidePanel *);

    private:

      void resetLayout();

      Graph * m_graph;
      QColor m_color;
      QPen m_pen;
      PortType m_portType;

      ProxyPort* m_proxyPort;
      std::vector<Port*> m_ports;
    };

  };

};

#endif // __UI_GraphView_SidePanel__
