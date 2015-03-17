// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Controller.h"
#include "Graph.h"
#include "Node.h"
#include "Pin.h"
#include "Port.h"
#include "Connection.h"
#include "ConnectionTarget.h"
#include "Commands/AddNodeCommand.h"
#include "Commands/RemoveNodeCommand.h"
#include "Commands/RenameNodeCommand.h"
#include "Commands/Command.h"
#include "Commands/AddPinCommand.h"
#include "Commands/RemovePinCommand.h"
#include "Commands/AddConnectionCommand.h"
#include "Commands/RemoveConnectionCommand.h"
#include "Commands/AddPortCommand.h"
#include "Commands/RemovePortCommand.h"
#include "Commands/RenamePortCommand.h"

using namespace FabricUI::GraphView;
using namespace FabricServices::Commands;

Controller::Controller(Graph * parent, CommandStack * stack)
{
  m_graph = parent;
  if(m_graph)
    m_graph->setController(this);
  m_stack = stack;
  m_compound = NULL;
  m_interactionBracket = 0;
}

Controller::~Controller()
{
  if(m_compound)
    delete(m_compound);
}

Graph * Controller::graph()
{
  return m_graph;
}

const Graph * Controller::graph() const
{
  return m_graph;
}

bool Controller::beginInteraction()
{
  if(m_interactionBracket == 0)
  {
    if(m_compound == NULL)
      m_compound = new CompoundCommand();
  }
  return m_interactionBracket++ == 0;
}

bool Controller::endInteraction()
{
  if(m_interactionBracket > 0)
  {
    m_interactionBracket--;
    if(m_interactionBracket == 0)
    {
      if(m_compound)
      {
        if(m_compound->isEmpty())
          delete(m_compound);
        else
          m_stack->add(m_compound);
        m_compound = NULL;
      }
      return true;
    }
  }
  return false;
}

Node * Controller::addNodeFromPreset(QString preset, QPointF pos)
{
  QString path;
  if(preset.length() == 0)
  {
    path = graph()->path() + graph()->config().pathSep + "Node";
  }
  else
  {
    QStringList parts = preset.split(graph()->config().pathSep);
    path = graph()->path() + graph()->config().pathSep + parts[parts.count()-1];
  }
  path = graph()->getUniquePath(path);

  AddNodeCommand * command = new AddNodeCommand(this, path, preset, pos);
  if(!addCommand(command))
  {
    delete(command);
    return NULL;
  }
  return command->getNode();
}

bool Controller::removeNode(Node * node)
{
  RemoveNodeCommand * command = new RemoveNodeCommand(this, node->path());
  if(!addCommand(command))
  {
    delete(command);
    return false;
  }
  return true;
}

bool Controller::moveNode(Node * node, QPointF pos, bool isTopLeftPos)
{
  if(isTopLeftPos)
    node->setTopLeftGraphPos(pos, false /* quiet */);
  else
    node->setGraphPos(pos, false /* quiet */);
  return true;
}

bool Controller::renameNode(Node * node, QString title)
{
  if(node->title() == title)
    return false;
  RenameNodeCommand * command = new RenameNodeCommand(this, node, title);
  if(!addCommand(command))
  {
    delete(command);
    return false;
  }
  return true;
}

bool Controller::selectNode(Node * node, bool state)
{
  if(node->selected() != state)
  {
    node->setSelected(state);
    return true;
  }
  return false;
}

bool Controller::clearSelection()
{
  if(!m_graph)
    return false;
  std::vector<Node*> nodes = m_graph->selectedNodes();
  for(size_t i=0;i<nodes.size();i++)
  {
    nodes[i]->setSelected(false);
  }
  return nodes.size() > 0;
}

Pin * Controller::addPin(Node * node, QString name, PortType pType, QColor color, QString dataType)
{
  AddPinCommand * command = new AddPinCommand(this, node, name, pType, color, dataType);

  if(addCommand(command))
    return command->getPin();

  delete(command);
  return NULL;
}

bool Controller::removePin(Pin * pin)
{
  RemovePinCommand * command = new RemovePinCommand(this, pin);

  if(addCommand(command))
    return true;

  delete(command);
  return false;
}

Port * Controller::addPort(QString name, PortType pType, QColor color, QString dataType)
{
  AddPortCommand * command = new AddPortCommand(this, name, pType, color, dataType);

  if(addCommand(command))
    return command->getPort();

  delete(command);
  return NULL;
}

bool Controller::removePort(Port * port)
{
  RemovePortCommand * command = new RemovePortCommand(this, port);

  if(addCommand(command))
    return true;

  delete(command);
  return false;
}

Port * Controller::addPortFromPin(Pin * pin, PortType pType)
{
  if(!m_graph)
    return NULL;
  beginInteraction();
  Port * port = addPort(pin->name(), pType, pin->color(), pin->dataType());
  if(port)
  {
    std::vector<Connection*> connections = m_graph->connections();
    if(pType == PortType_Output)
    {
      for(size_t i=0;i<connections.size();i++)
      {
        if(connections[i]->dst() == pin)
        {
          if(!removeConnection(connections[i]))
          {
            endInteraction();
            return NULL;
          }
          break;
        }
      }
      addConnection(port, pin);
    }
    else if(pType == PortType_Input)
    {
      for(size_t i=0;i<connections.size();i++)
      {
        if(connections[i]->dst() == port)
        {
          if(!removeConnection(connections[i]))
          {
            endInteraction();
            return NULL;
          }
          break;
        }
      }
      addConnection(pin, port);
    }
  }
  endInteraction();
  return port;
}

bool Controller::renamePort(Port * port, QString title)
{
  if(port->name() == title)
    return false;
  RenamePortCommand * command = new RenamePortCommand(this, port, title);
  if(!addCommand(command))
  {
    delete(command);
    return false;
  }
  return true;
}

bool Controller::addConnection(ConnectionTarget * src, ConnectionTarget * dst)
{
  Command * command = new AddConnectionCommand(this, src, dst);;
  if(addCommand(command))
    return true;
  delete(command);
  return false;
}

bool Controller::removeConnection(ConnectionTarget * src, ConnectionTarget * dst)
{
  Command * command = new RemoveConnectionCommand(this, src, dst);;
  if(addCommand(command))
    return true;
  delete(command);
  return false;
}

bool Controller::removeConnection(Connection * conn)
{
  return removeConnection(conn->src(), conn->dst());  
}

bool Controller::zoomCanvas(float zoom)
{
  if(!m_graph)
    return false;
  m_graph->mainPanel()->setCanvasZoom(zoom, true);
  return true;
}

bool Controller::panCanvas(QPointF pan)
{
  if(!m_graph)
    return false;
  m_graph->mainPanel()->setCanvasPan(pan, true);
  return true;
}

bool Controller::frameNodes(const std::vector<Node*> & nodes)
{
  if(!m_graph)
    return false;
  if(nodes.size() == 0)
    return false;

  QRectF bounds;
  for(size_t i=0;i<nodes.size();i++)
    bounds = bounds.united(nodes[i]->boundingRect().translated(nodes[i]->topLeftGraphPos()));

  return panCanvas(m_graph->mainPanel()->boundingRect().center() - bounds.center() * m_graph->mainPanel()->canvasZoom());
}

bool Controller::frameSelectedNodes()
{
  return frameNodes(m_graph->selectedNodes());
}

bool Controller::frameAllNodes()
{
  return frameNodes(m_graph->nodes());
}

void Controller::populateNodeToolbar(NodeToolbar * toolbar, Node * node)
{
  toolbar->addTool("node_collapse", "node_collapse.png");
  toolbar->setToolRotation("node_collapse", (int)node->collapsedState());
}

bool Controller::canConnect(QString pathA, QString pathB, QString &failureReason)
{
  if(!m_graph)
  {
    failureReason = "no graph";
    return false;
  }
  QString relPathA = GraphView::relativePath(m_graph->path(), pathA);
  QString relPathB = GraphView::relativePath(m_graph->path(), pathB);
  QString dataTypeA;
  QString dataTypeB;

  if(relPathA.indexOf('.') == -1)
  {
    GraphView::Port * port = m_graph->port(relPathA);
    if(port)
      dataTypeA = port->dataType();
  }
  else
  {
    QStringList parts = relPathA.split('.');
    if(parts.length() == 2)
    {
      GraphView::Node * node = m_graph->node(parts[0]);
      if(node)
      {
        GraphView::Pin * pin = node->pin(parts[1]);
        if(pin)
        {
          dataTypeA = pin->dataType();
        }
      }
    }
  }

  if(relPathB.indexOf('.') == -1)
  {
    GraphView::Port * port = m_graph->port(relPathB);
    if(port)
      dataTypeB = port->dataType();
  }
  else
  {
    QStringList parts = relPathB.split('.');
    if(parts.length() == 2)
    {
      GraphView::Node * node = m_graph->node(parts[0]);
      if(node)
      {
        GraphView::Pin * pin = node->pin(parts[1]);
        if(pin)
        {
          dataTypeB = pin->dataType();
        }
      }
    }
  }

  if(dataTypeA == dataTypeB)
    return true;

  if(dataTypeA == "" || dataTypeB == "")
    return true;

  failureReason = "type mismatch";
  return false;
}

bool Controller::addCommand(Command * command)
{
  if(!command)
    return false;
  if(m_compound)
    return m_compound->add(command);
  return m_stack->add(command);
}

bool Controller::clearCommands()
{
  m_stack->clear();
  return true;
}

void Controller::nodeToolTriggered(FabricUI::GraphView::Node * node, QString toolName)
{
  if(toolName == "node_collapse")
  {
    node->toggleCollapsedState();
  }
}
