// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeHeader__
#define __UI_GraphView_NodeHeader__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include "ConnectionTarget.h"
#include "PinCircle.h"
#include "NodeHeaderButton.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Node;
    class NodeLabel;
    class NodeEditButton;

    class NodeHeader : public ConnectionTarget
    {
      Q_OBJECT

    public:

      NodeHeader(
        Node * parent,
        QString const &title
        );

      Node * node();
      const Node * node() const;
      NodeLabel * labelWidget() { return m_title; }

      void setTitle(QString const &title);

      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);

      virtual std::string path() const;

      // ConnectionTarget
      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const { return true; }

      virtual TargetType targetType() const { return TargetType_NodeHeader; }
      virtual QPointF connectionPos(PortType pType) const;
    
      virtual Graph * graph();
      virtual const Graph * graph() const;
      virtual QColor color() const;
      void setColor(QColor col);

      bool areCirclesVisible() const;
      void setCirclesVisible(bool visible);

      void addHeaderButton(QString name, QString icon);
      void setHeaderButtonRotation(QString name, int rotation);

    signals:

      void headerButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

    private slots:

      void onHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

    private:

      Node * m_node;
      NodeLabel * m_title;
      NodeEditButton * m_editButton;
      PinCircle * m_inCircle;
      PinCircle * m_outCircle;
      std::vector<NodeHeaderButton*> m_buttons;
    };

  };

};

#endif // __UI_GraphView_NodeHeader__
