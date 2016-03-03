// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_InfoOverlay__
#define __UI_GraphView_InfoOverlay__

#include "Node.h"
#include "GraphConfig.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;

    class InfoOverlay : public QGraphicsWidget
    {
      Q_OBJECT

      friend class Node;

    public:

      InfoOverlay(Graph * parent, QString text, const GraphConfig & config = GraphConfig());
      virtual ~InfoOverlay();

      Graph * graph();
      const Graph * graph() const;

      virtual QString text() const;
      virtual void setText(QString t);

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    protected:

      void updateSize();

      Graph * m_graph;
      GraphConfig m_config;
      QString m_text;
      QStringList m_textLines;
      QFontMetrics * m_metrics;

    };


  };

};

#endif // __UI_GraphView_InfoOverlay__
