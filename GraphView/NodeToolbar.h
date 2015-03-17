// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeToolbar__
#define __UI_GraphView_NodeToolbar__

#include "Node.h"
#include "GraphConfig.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;

    class NodeToolbar : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      NodeToolbar(Graph * parent, const GraphConfig & config = GraphConfig());
      virtual ~NodeToolbar();

      Graph * graph();
      const Graph * graph() const;

      Node * node();
      const Node * node() const;

      virtual void attach(Node * node);
      virtual void deattach(bool disconnectSignal = true);

      virtual void addTool(QString name, QString resource, bool performUpdate = false);
      virtual void clearTools();

      virtual int toolRotation(QString name) const;
      virtual void setToolRotation(QString name, int rotation);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    public slots:

      void onNodePositionChanged(FabricUI::GraphView::Node *, QPointF);

    signals:

      void toolTriggered(FabricUI::GraphView::Node *, QString);

    protected:

      QRectF toolRect(int index) const;

      struct Tool
      {
        QString name;
        QPixmap pixmap;
        int rotation;
      };

      Graph * m_graph;
      Node * m_node;
      GraphConfig m_config;
      std::vector<Tool> m_tools;

      static std::map<QString, QPixmap> s_pixmaps;
    };


  };

};

#endif // __UI_GraphView_NodeToolbar__
