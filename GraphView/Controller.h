// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Controller__
#define __UI_GraphView_Controller__

#include <QtCore/QString>
#include <QtCore/QPointF>
#include <QtGui/QColor>

#include <Commands/CommandStack.h>
#include <Commands/CompoundCommand.h>

#include "PortType.h"
#include "NodeToolbar.h"

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
    class BackDropNode;
    class Pin;
    class Port;
    class Connection;
    class ConnectionTarget;

    class Controller : public QObject
    {
      Q_OBJECT
      
    public:

      Controller(Graph * graph, FabricServices::Commands::CommandStack * stack);
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

      virtual Node * addNode(
        FTL::CStrRef name,
        FTL::CStrRef title,
        QPointF pos
        );
      virtual bool moveNode(Node * node, QPointF pos, bool isTopLeftPos = false);
      virtual bool selectNode(Node * node, bool state);
      virtual bool clearSelection();
      virtual Pin * addPin(
        GraphView::Node * node,
        FTL::CStrRef name,
        GraphView::PortType pType,
        QColor color,
        FTL::CStrRef dataType = ""
        );
      virtual bool removePin(Pin * pin);
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool frameNodes(const std::vector<Node*> & nodes);
      virtual bool frameSelectedNodes();
      virtual bool frameAllNodes();
      virtual void populateNodeToolbar(NodeToolbar * toolbar, Node * node);
      virtual bool setBackDropNodeSize(BackDropNode * node, QSizeF size);

      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        );

      bool gvcDoRemoveConnection(Connection * conn);

      bool addCommand(FabricServices::Commands::Command * command);
      bool clearCommands();

    public slots:

      void nodeToolTriggered(FabricUI::GraphView::Node *, char const *);

    private:

      Graph * m_graph;
      unsigned int m_interactionBracket;
      FabricServices::Commands::CommandStack * m_stack;
      FabricServices::Commands::CompoundCommand * m_compound;
    };

  };

};

#endif // __UI_GraphView_Controller__
