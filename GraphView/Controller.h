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

      virtual bool beginInteraction();
      virtual bool endInteraction();
      virtual Node * addNodeFromPreset(QString preset, QPointF pos);
      virtual bool removeNode(Node * node);
      virtual bool moveNode(Node * node, QPointF pos, bool isTopLeftPos = false);
      virtual bool renameNode(Node * node, QString title);
      virtual bool selectNode(Node * node, bool state);
      virtual bool clearSelection();
      virtual Pin * addPin(Node * node, QString name, PortType pType, QColor color, QString dataType = "");
      virtual bool removePin(Pin * pin);
      virtual Port * addPort(QString name, PortType pType, QColor color, QString dataType = "");
      virtual bool removePort(Port * port);
      virtual Port * addPortFromPin(Pin * pin, PortType pType);
      virtual bool renamePort(Port * port, QString title);
      virtual bool addConnection(ConnectionTarget * src, ConnectionTarget * dst);
      virtual bool removeConnection(ConnectionTarget * src, ConnectionTarget * dst);
      virtual bool removeConnection(Connection * conn);
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool frameNodes(const std::vector<Node*> & nodes);
      virtual bool frameSelectedNodes();
      virtual bool frameAllNodes();
      virtual void populateNodeToolbar(NodeToolbar * toolbar, Node * node);

      virtual bool canConnect(QString pathA, QString pathB, QString &failureReason);

      bool addCommand(FabricServices::Commands::Command * command);
      bool clearCommands();

    public slots:

      void nodeToolTriggered(FabricUI::GraphView::Node *, QString);

    private:

      Graph * m_graph;
      unsigned int m_interactionBracket;
      FabricServices::Commands::CommandStack * m_stack;
      FabricServices::Commands::CompoundCommand * m_compound;
    };

  };

};

#endif // __UI_GraphView_Controller__
