// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeHeader__
#define __UI_GraphView_NodeHeader__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Node;
    class NodeLabel;
    class NodeEditButton;

    class NodeHeader : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      NodeHeader(Node * parent, QString text);

      Node * node();
      const Node * node() const;

      QString title() const;
      void setTitle(QString t);

      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);
      
    private:

      Node * m_node;
      NodeLabel * m_title;
      NodeEditButton * m_editButton;
    };

  };

};

#endif // __UI_GraphView_NodeHeader__
