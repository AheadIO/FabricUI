// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_SidePanel__
#define __UI_GraphView_SidePanel__

#include <FabricCore.h>

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsLinearLayout>
#include <vector>

#include "PortType.h"
#include "Port.h"
#include "ProxyPort.h"
#include "SidePanelItemGroup.h"

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
      friend class MainPanel;

    public:

      SidePanel(Graph * parent, PortType portType, QColor color = QColor());

      Graph * graph();
      const Graph * graph() const;
      QGraphicsWidget * itemGroup();
      const QGraphicsWidget * itemGroup() const;

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
      virtual void wheelEvent(QGraphicsSceneWheelEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

      virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *event );

      // accessed through controller
      virtual Port * addPort(Port * port);
      virtual bool removePort(Port * port);
      virtual void reorderPorts(QStringList names);

      void scroll(float delta);
      void updateItemGroupScroll(float height = 0.0f);

    signals:
      void doubleClicked(FabricUI::GraphView::SidePanel *);
      void scrolled();

    private slots:
      void onItemGroupResized();

    private:

      void resetLayout();

      Graph * m_graph;
      QColor m_color;
      QPen m_pen;
      PortType m_portType;
      bool m_requiresToSendSignalsForPorts;
      SidePanelItemGroup * m_itemGroup;
      float m_itemGroupScroll;

      ProxyPort* m_proxyPort;
      std::vector<Port*> m_ports;
    };

  };

};

#endif // __UI_GraphView_SidePanel__
