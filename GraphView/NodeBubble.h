// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeBubble__
#define __UI_GraphView_NodeBubble__

#include "Node.h"
#include "GraphConfig.h"

class QGraphicsRectItem;
class QGraphicsSimpleTextItem;

namespace FabricUI {
namespace GraphView {

class Graph;

class NodeBubble : public QGraphicsObject
{
  Q_OBJECT

  friend class Node;

public:

  NodeBubble( Node *node, GraphConfig const &config = GraphConfig() );
  virtual ~NodeBubble();

  virtual QRectF boundingRect() const;

  void expand();
  void collapse();

  bool isCollapsed() const;
  bool isExpanded() const;

  QString text() const;
  void setText( QString t );

public slots:

  void onNodePositionChanged(FabricUI::GraphView::Node *, QPointF);

signals:

  void bubbleEditRequested( FabricUI::GraphView::NodeBubble *bubble );

protected:

  virtual void paint(
    QPainter *painter,
    QStyleOptionGraphicsItem const *option,
    QWidget *widget
    );
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

  void updateChildrenGeometries();

  Node *m_node;
  qreal m_nodeCornerRadius, m_nodeWidthReduction;
  QSizeF m_minSize;
  QGraphicsRectItem *m_rectItem;
  QGraphicsSimpleTextItem *m_textItem;
  
  static QSizeF const s_topLeftTextMargins, s_bottomRightTextMargins;
};

} // namespace GraphView
} // namespace FabricUI

#endif // __UI_GraphView_NodeBubble__
