// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/ConnectionTarget.h>

using namespace FabricUI::GraphView;
using namespace FabricServices::Commands;

Controller::Controller(Graph * parent)
{
  m_graph = parent;
  if(m_graph)
    m_graph->setController(this);
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
        delete m_compound;
        m_compound = NULL;
      }
      return true;
    }
  }
  return false;
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

bool Controller::frameNodes(const std::vector<Node*> & nodes, float zoom)
{
  if(!m_graph)
    return false;
  if(nodes.size() == 0)
    return false;

  QRectF bounds;
  for(size_t i=0;i<nodes.size();i++)
    bounds = bounds.united(nodes[i]->boundingRect().translated(nodes[i]->topLeftGraphPos()));

  if(zoom != 0.0f)
    zoomCanvas(zoom);

  bool result = panCanvas(m_graph->mainPanel()->boundingRect().center() - bounds.center() * m_graph->mainPanel()->canvasZoom());

  m_graph->mainPanel()->update();

  return result;
}

bool Controller::frameSelectedNodes()
{
  return frameNodes(m_graph->selectedNodes(), 1.0);
}

bool Controller::frameAllNodes()
{
  return frameNodes(m_graph->nodes());
}

void Controller::onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button)
{
  if(button->name() == "node_collapse")
  {
    int state = (button->state() + 1) % 3;
    button->header()->node()->setCollapsedState((Node::CollapseState)state);
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
