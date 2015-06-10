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

void Controller::setGraph(Graph * graph)
{
  m_graph = graph;
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

Node * Controller::addNodeFromPreset(FTL::CStrRef preset, QPointF pos)
{
  std::string presetStr(preset);
  std::string path;
  if(presetStr.length() == 0)
  {
    path += "Node";
  }
  else
  {
    int pos =  presetStr.find(graph()->config().pathSep);
    if(pos == std::string::npos)
      path += presetStr;
    else
      path += presetStr.substr(pos+1, presetStr.length());
  }

  AddNodeCommand * command =
    new AddNodeCommand(this, path.c_str(), preset.data(), pos);
  if(!addCommand(command))
  {
    delete(command);
    return NULL;
  }
  return command->getNode();
}

bool Controller::removeNode(Node * node)
{
  RemoveNodeCommand * command =
    new RemoveNodeCommand(this, node->name().c_str());
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

bool Controller::renameNode(Node * node, FTL::StrRef title)
{
  if(title == node->title())
    return false;
  RenameNodeCommand * command = new RenameNodeCommand(this, node, title.data());
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

Pin * Controller::addPin(Node * node, FTL::StrRef name, PortType pType, QColor color, FTL::StrRef dataType)
{
  AddPinCommand * command = new AddPinCommand(this, node, name.data(), pType, color, dataType.data());

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

Port * Controller::addPort(FTL::StrRef name, PortType pType, QColor color, FTL::StrRef dataType)
{
  AddPortCommand * command = new AddPortCommand(this, name.data(), pType, color, dataType.data());

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

bool Controller::renamePort(Port * port, FTL::StrRef title)
{
  if(title == port->name())
    return false;
  RenamePortCommand * command = new RenamePortCommand(this, port, title.data());
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

  bool result = panCanvas(m_graph->mainPanel()->boundingRect().center() - bounds.center() * m_graph->mainPanel()->canvasZoom());

  m_graph->mainPanel()->update();

  return result;
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

void Controller::nodeToolTriggered(FabricUI::GraphView::Node * node, char const * toolName)
{
  FTL::StrRef toolNameRef(toolName);
  if(toolNameRef == "node_collapse")
  {
    node->toggleCollapsedState();
    if(m_graph)
    {
      if(m_graph->nodeToolbar())
      {
        m_graph->nodeToolbar()->setToolRotation(toolName, node->collapsedState());
      }
    }    
  }
}
