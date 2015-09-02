// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Controller__
#define __UI_GraphView_Controller__

#include <QtCore/QString>
#include <QtCore/QPointF>
#include <QtCore/QSizeF>
#include <QtGui/QColor>

#include <Commands/CommandStack.h>
#include <Commands/CompoundCommand.h>

#include "PortType.h"

#include <vector>

#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;
    class Node;
    class NodeHeader;
    class NodeHeaderButton;
    class BackDropNode;
    class Pin;
    class Port;
    class Connection;
    class ConnectionTarget;

    class Controller : public QObject
    {
      Q_OBJECT
      
    public:

      Controller(Graph * graph);
      virtual ~Controller();

      Graph * graph();
      const Graph * graph() const;
      void setGraph(Graph * graph);

      virtual bool beginInteraction();
      virtual bool endInteraction();

      virtual bool gvcDoRemoveNodes(
        FTL::ArrayRef<GraphView::Node *> nodes
        ) = 0;

      virtual bool gvcDoAddConnection(
        ConnectionTarget * src,
        ConnectionTarget * dst
        ) = 0;

      virtual bool gvcDoRemoveConnection(
        ConnectionTarget * src,
        ConnectionTarget * dst
        ) = 0;

      virtual bool gvcDoAddInstFromPreset(
        FTL::CStrRef presetPath,
        QPointF pos
        ) = 0;

      virtual void gvcDoAddPort(
        FTL::CStrRef desiredPortName,
        PortType portType,
        FTL::CStrRef typeSpec = FTL::CStrRef(),
        ConnectionTarget *connectWith = 0
        ) = 0;
      
      virtual void gvcDoSetNodeCommentExpanded(
        Node *node,
        bool expanded
        ) = 0;

      virtual void gvcDoMoveNodes(
        std::vector<Node *> const &nodes,
        QPointF delta,
        bool allowUndo
        ) = 0;

      virtual void gvcDoResizeBackDropNode(
        GraphView::BackDropNode *backDropNode,
        QPointF posDelta,
        QSizeF sizeDelta,
        bool allowUndo
        ) = 0;

      virtual bool selectNode(Node * node, bool state);
      virtual bool clearSelection();
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool frameNodes(const std::vector<Node*> & nodes, float zoom = 0.0f);
      virtual bool frameSelectedNodes();
      virtual bool frameAllNodes();

      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        );

      bool gvcDoRemoveConnection(Connection * conn);

    public slots:
      virtual void onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

    private:

      Graph * m_graph;
      unsigned int m_interactionBracket;
      FabricServices::Commands::CompoundCommand * m_compound;
    };

  };

};

#endif // __UI_GraphView_Controller__
