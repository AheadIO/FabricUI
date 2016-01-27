// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeRectangle__
#define __UI_GraphView_NodeRectangle__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include "GraphicItemTypes.h"

namespace FabricUI
{

  namespace GraphView
  {

    // forward declarations
    class Node;

    class NodeRectangle : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      NodeRectangle(Node * parent);
      virtual ~NodeRectangle() {}

      virtual int type() const { return QGraphicsItemType_NodeRectangle; }

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    private:

      Node * m_node;

    };


  };

};

#endif // __UI_GraphView_NodeRectangle__
