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
#include <FTL/StrRef.h>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;
    class Node;
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
      virtual Node * addNodeFromPreset(FTL::CStrRef preset, QPointF pos);
      virtual bool removeNode(Node * node);
      virtual bool moveNode(Node * node, QPointF pos, bool isTopLeftPos = false);
      virtual bool renameNode(Node * node, FTL::StrRef title);
      virtual bool selectNode(Node * node, bool state);
      virtual bool clearSelection();
      virtual Pin * addPin(Node * node, FTL::StrRef name, PortType pType, QColor color, FTL::StrRef dataType = "");
      virtual bool removePin(Pin * pin);
      virtual Port * addPort(FTL::StrRef name, PortType pType, QColor color, FTL::StrRef dataType = "");
      virtual bool removePort(Port * port);
      virtual Port * addPortFromPin(Pin * pin, PortType pType);
      virtual bool renamePort(Port * port, FTL::StrRef title);
      virtual bool addConnection(ConnectionTarget * src, ConnectionTarget * dst);
      virtual bool removeConnection(ConnectionTarget * src, ConnectionTarget * dst);
      virtual bool removeConnection(Connection * conn);
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool frameNodes(const std::vector<Node*> & nodes);
      virtual bool frameSelectedNodes();
      virtual bool frameAllNodes();
      virtual void populateNodeToolbar(NodeToolbar * toolbar, Node * node);

      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        );

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
