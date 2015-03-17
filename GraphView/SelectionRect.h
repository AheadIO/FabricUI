// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_SelectionRect__
#define __UI_GraphView_SelectionRect__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QPen>
#include <QtGui/QColor>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class MainPanel;
    class Graph;

    class SelectionRect : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      SelectionRect(MainPanel * parent, QPointF mouseDownPos);
      virtual ~SelectionRect() {}

      Graph * graph();
      const Graph * graph() const;

      void setDragPoint(QPointF dragPoint);

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    private:

      MainPanel * m_mainPanel;
      QColor m_color;
      QPen m_pen;
      QPointF m_mouseDownPos; 
    };

  };

};

#endif // __UI_GraphView_SelectionRect__
