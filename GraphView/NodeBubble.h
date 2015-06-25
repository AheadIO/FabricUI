// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeBubble__
#define __UI_GraphView_NodeBubble__

#include "Node.h"
#include "GraphConfig.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;

    class NodeBubble : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      NodeBubble(Graph * parent, Node * node, const GraphConfig & config = GraphConfig());
      virtual ~NodeBubble();

      Graph * graph();
      const Graph * graph() const;

      Node * node();
      const Node * node() const;

      virtual void expand();
      virtual void collapse();
      virtual bool expanded() const { return !m_collapsed; }
      virtual bool collapsed() const { return m_collapsed; }

      virtual QString text() const;
      virtual void setText(QString t);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    public slots:

      void onNodePositionChanged(FabricUI::GraphView::Node *, QPointF);
      void requestEdit();

    signals:

      void bubbleExpanded(FabricUI::GraphView::NodeBubble * bubble);
      void bubbleCollapsed(FabricUI::GraphView::NodeBubble * bubble);
      void bubbleTextChanged(FabricUI::GraphView::NodeBubble * bubble, QString text);
      void bubbleEditRequested(FabricUI::GraphView::NodeBubble * bubble);

    protected:

      void updateSize();

      bool m_collapsed;
      Graph * m_graph;
      Node * m_node;
      GraphConfig m_config;
      QString m_text;
      QStringList m_textLines;
      QFontMetrics * m_metrics;

    };


  };

};

#endif // __UI_GraphView_NodeBubble__
