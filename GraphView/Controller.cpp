// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/ConnectionTarget.h>
#include <FabricUI/GraphView/Commands/AddNodeCommand.h>
#include <FabricUI/GraphView/Commands/RenameNodeCommand.h>
#include <FabricUI/GraphView/Commands/AddPinCommand.h>
#include <FabricUI/GraphView/Commands/RemovePinCommand.h>
#include <FabricUI/GraphView/Commands/AddPortCommand.h>
#include <FabricUI/GraphView/Commands/RemovePortCommand.h>
#include <FabricUI/GraphView/Commands/RenamePortCommand.h>

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

Node * Controller::addNode(
  FTL::CStrRef name,
  FTL::CStrRef title,
  QPointF pos
  )
{
  AddNodeCommand * command = new AddNodeCommand(this, name, title, pos);
  if(!addCommand(command))
  {
    delete(command);
    return NULL;
  }
  return command->getNode();
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

bool Controller::gvcDoRemoveConnection(Connection * conn)
{
  return gvcDoRemoveConnection(conn->src(), conn->dst());  
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

bool Controller::canConnectTo(
  char const *pathA,
  char const *pathB,
  std::string &failureReason
  )
{
  return true;
}
