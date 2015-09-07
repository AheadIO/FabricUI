// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/Exception.h>

using namespace FabricUI::GraphView;

Graph::Graph(
  QGraphicsItem * parent,
  const GraphConfig & config
  )
  : QGraphicsWidget(parent)
  , m_config( config )
{
  m_isEditable = true;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  setMinimumSize(400, 400);
  setContentsMargins(0, 0, 0, 0);

  m_controller = NULL;
  m_mouseGrabber = NULL;
  m_mainPanel = NULL;
  m_leftPanel = NULL;
  m_rightPanel = NULL;
  m_graphContextMenuCallback = NULL;
  m_nodeContextMenuCallback = NULL;
  m_pinContextMenuCallback = NULL;
  m_connectionContextMenuCallback = NULL;
  m_portContextMenuCallback = NULL;
  m_sidePanelContextMenuCallback = NULL;
  m_graphContextMenuCallbackUD = NULL;
  m_nodeContextMenuCallbackUD = NULL;
  m_pinContextMenuCallbackUD = NULL;
  m_connectionContextMenuCallbackUD = NULL;
  m_portContextMenuCallbackUD = NULL;
  m_sidePanelContextMenuCallbackUD = NULL;
  m_overlayItem = NULL;
  m_maxZValue = 0.1;
}

void Graph::requestSidePanelInspect(
  FabricUI::GraphView::SidePanel *sidePanel
  )
{
  if ( m_isEditable )
    emit sidePanelInspectRequested();
}

void Graph::initialize()
{
  m_mainPanel = new MainPanel(this);

  m_leftPanel = new SidePanel(this, PortType_Output);
  QObject::connect(
    m_leftPanel, SIGNAL(doubleClicked(FabricUI::GraphView::SidePanel*)), 
    this, SLOT(requestSidePanelInspect(FabricUI::GraphView::SidePanel*))
    );

  m_rightPanel = new SidePanel(this, PortType_Input);
  QObject::connect(
    m_rightPanel, SIGNAL(doubleClicked(FabricUI::GraphView::SidePanel*)), 
    this, SLOT(requestSidePanelInspect(FabricUI::GraphView::SidePanel*))
    );

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setOrientation(Qt::Horizontal);
  layout->addItem(m_leftPanel);
  layout->addItem(m_mainPanel);
  layout->addItem(m_rightPanel);
  setLayout(layout);
}

const GraphConfig & Graph::config() const
{
  return m_config;
}

Controller * Graph::controller()
{
  return m_controller;
}

void Graph::setController(Controller * c)
{
  m_controller = c;
}

QGraphicsWidget * Graph::itemGroup()
{
  return m_mainPanel->itemGroup();
}

bool Graph::hasSidePanels() const
{
  return m_leftPanel != NULL;
}

MainPanel * Graph::mainPanel()
{
  return m_mainPanel;
}

const MainPanel * Graph::mainPanel() const
{
  return m_mainPanel;
}

SidePanel * Graph::sidePanel(PortType portType)
{
  if(portType == PortType_Output)
    return m_leftPanel;
  if(portType == PortType_Input)
    return m_rightPanel;
  return NULL;
}

const SidePanel * Graph::sidePanel(PortType portType) const
{
  if(portType == PortType_Input)
    return m_leftPanel;
  if(portType == PortType_Output)
    return m_rightPanel;
  return NULL;
}

Node * Graph::addNode(Node * node, bool quiet)
{
  FTL::StrRef key = node->name();
  if(m_nodeMap.find(key) != m_nodeMap.end())
    return NULL;

  m_nodeMap.insert(std::pair<FTL::StrRef, size_t>(key, m_nodes.size()));
  m_nodes.push_back(node);

  double zValue = m_maxZValue;
  if(!node->isBackDropNode())
    zValue += 3.0;
  node->setZValue(zValue);
  m_maxZValue += 0.0001;
  if(node->bubble())
  {
    m_maxZValue += 0.0001;
    node->bubble()->setZValue(zValue + 0.0001);
  }

  QObject::connect(
    node, 
    SIGNAL(doubleClicked(FabricUI::GraphView::Node*, Qt::MouseButton, Qt::KeyboardModifiers)), 
    this, 
    SLOT(onNodeDoubleClicked(FabricUI::GraphView::Node*, Qt::MouseButton, Qt::KeyboardModifiers))
    );
  QObject::connect(node, SIGNAL(bubbleEditRequested(FabricUI::GraphView::Node*)), this, SLOT(onBubbleEditRequested(FabricUI::GraphView::Node*)));

  if(!quiet)
    emit nodeAdded(node);

  return node;
}

Node * Graph::addNode(
  FTL::CStrRef name,
  FTL::CStrRef title,
  bool quiet
  )
{
  return addNode(new Node(this, name, title), quiet);
}

BackDropNode *Graph::addBackDropNode(
  FTL::CStrRef name,
  bool quiet
  )
{
  BackDropNode *backDropNode = new BackDropNode( this, name );
  addNode( backDropNode, quiet );
  return backDropNode;
}

bool Graph::removeNode(Node * node, bool quiet)
{
  FTL::StrRef key = node->name();
  std::map<FTL::StrRef, size_t>::iterator it = m_nodeMap.find(key);
  if(it == m_nodeMap.end())
    return false;

  controller()->beginInteraction();

  for(int i=m_connections.size()-1;i>=0;i--)
  {
    Connection * con = m_connections[i];
    bool found = false;
    for(int j=0;j<2;j++)
    {
      ConnectionTarget * target = j == 0 ? con->src() : con->dst();
      if(target->targetType() == TargetType_Pin)
      {
        Pin * pin = (Pin *)target;
        if(pin->node() == node)
        {
          found = true;
          break;
        }
      }
    }

    if(found)
      controller()->gvcDoRemoveConnection(con);
  }

  size_t index = it->second;
  m_nodes.erase(m_nodes.begin() + index);
  m_nodeMap.erase(it);

  // update the lookup
  for(it = m_nodeMap.begin(); it != m_nodeMap.end(); it++)
  {
    if(it->second > index)
      it->second--;
  }

  if(!quiet)
    emit nodeRemoved(node);

  prepareGeometryChange();
  scene()->removeItem(node);
  delete(node);

  controller()->endInteraction();

  return true;
}

std::vector<Node *> Graph::nodes() const
{
  std::vector<Node *> result;
  result.insert(result.end(), m_nodes.begin(), m_nodes.end());
  return result;
}

Node * Graph::node( FTL::StrRef name ) const
{
  std::map<FTL::StrRef, size_t>::const_iterator it = m_nodeMap.find(name);
  if(it == m_nodeMap.end())
    return NULL;
  return m_nodes[it->second];
}

std::vector<Node *> Graph::selectedNodes() const
{
  std::vector<Node *> result;
  for(size_t i=0;i<m_nodes.size();i++)
  {
    if(m_nodes[i]->selected())
      result.push_back(m_nodes[i]);
  }
  return result;
}

void Graph::clearSelection() const
{
  for (size_t i=0;i<m_nodes.size();i++)
    m_nodes[i]->setSelected( false );
}

bool Graph::addPort(Port * port, bool quiet)
{
  return port->sidePanel()->addPort(port) != NULL;
}

bool Graph::removePort(Port * port, bool quiet)
{
  return port->sidePanel()->removePort(port);
}

std::vector<Port *> Graph::ports() const
{
  std::vector<Port *> result;
  if(hasSidePanels())
  {
    for(unsigned int i=0;i<m_leftPanel->portCount();i++)
      result.push_back(m_leftPanel->port(i));
    for(unsigned int i=0;i<m_rightPanel->portCount();i++)
      result.push_back(m_rightPanel->port(i));
  }
  return result;
}

Port * Graph::port(FTL::StrRef name) const
{
  if(!hasSidePanels())
    return NULL;

  for(unsigned int i=0;i<m_leftPanel->portCount();i++)
  {
    if(name == m_leftPanel->port(i)->name())
      return m_leftPanel->port(i);
  }
  for(unsigned int i=0;i<m_rightPanel->portCount();i++)
  {
    if(name == m_rightPanel->port(i)->name())
      return m_rightPanel->port(i);
  }

  return NULL;
}

std::vector<Connection *> Graph::connections() const
{
  return m_connections;
}

bool Graph::isConnected(const ConnectionTarget * target) const
{
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->dst() == target || m_connections[i]->src() == target)
      return true;
  }
  return false;
}

void Graph::updateColorForConnections(const ConnectionTarget * target) const
{
  if(!m_config.connectionUsePinColor)
    return;
  if(target == NULL)
    return;

  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->dst() == target || m_connections[i]->src() == target)
    {
      m_connections[i]->setColor(target->color());
      m_connections[i]->update();
    }
  }
}

Connection * Graph::addConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet)
{
  if(src == dst)
    return NULL;

  if(src->targetType() == TargetType_MouseGrabber || dst->targetType() == TargetType_MouseGrabber)
    return NULL;

  // make sure this connection does not exist yet
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->src() == src && m_connections[i]->dst() == dst)
      return NULL;
  }

  if(m_config.disconnectInputsAutomatically)
  {
    for(size_t i=0;i<m_connections.size();i++)
    {
      if(m_connections[i]->dst() == dst)
      {
        // filter out IO ports
        if(m_connections[i]->src()->targetType() == TargetType_Port && m_connections[i]->dst()->targetType() == TargetType_Port)
        {
          if(((Port*)m_connections[i]->src())->name() == ((Port*)m_connections[i]->dst())->name())
            continue;
        }

        if(!controller()->gvcDoRemoveConnection(m_connections[i]))
          return NULL;
        break;
      }
    }
  }

  prepareGeometryChange();
  controller()->beginInteraction();

  Connection * connection = new Connection(this, src, dst);
  m_connections.push_back(connection);

  if(connection->src()->targetType() == TargetType_Pin)
  {
    Pin * pin = (Pin*)connection->src();
    pin->setDaisyChainCircleVisible(true);
    Node * node = pin->node();
    node->onConnectionsChanged();
  }
  if(connection->dst()->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)connection->dst())->node();
    node->onConnectionsChanged();
  }

  if(!quiet)
    emit connectionAdded(connection);

  controller()->endInteraction();

  return connection;
}

bool Graph::removeConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet)
{
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i]->src() == src && m_connections[i]->dst() == dst)
    {
      return removeConnection(m_connections[i], quiet);
    }
  }
  return false;
}

bool Graph::removeConnection(Connection * connection, bool quiet)
{
  bool found = false;
  size_t index;
  for(size_t i=0;i<m_connections.size();i++)
  {
    if(m_connections[i] == connection)
    {
      found = true;
      index = i;
      break;
    }
  }
  if(!found)
    return false;

  prepareGeometryChange();
  controller()->beginInteraction();

  Pin * daisyChainPin = NULL;
  if(connection->src()->targetType() == TargetType_Pin)
  {
    daisyChainPin = (Pin*)connection->src();
  }

  ConnectionTarget * src = connection->src();
  ConnectionTarget * dst = connection->dst();

  if(connection->dst()->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)connection->dst())->node();
    node->onConnectionsChanged();
  }

  m_connections.erase(m_connections.begin() + index);
  if(!quiet)
    emit connectionRemoved(connection);

  if(daisyChainPin)
  {
    bool found = false;
    for(size_t i=0;i<m_connections.size();i++)
    {
      if(m_connections[i]->src() == daisyChainPin)
      {
        found = true;
        break;
      }
    }
    daisyChainPin->setDaisyChainCircleVisible(found);
  }

  prepareGeometryChange();
  connection->invalidate();
  scene()->removeItem(connection);
  delete(connection);

  if(src->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)src)->node();
    node->onConnectionsChanged();
  }
  if(dst->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)dst)->node();
    node->onConnectionsChanged();
  }

  controller()->endInteraction();

  return true;
}


MouseGrabber * Graph::constructMouseGrabber(QPointF pos, ConnectionTarget * target, PortType portType)
{
  if(!m_isEditable)
    return NULL;
  m_mouseGrabber = MouseGrabber::construct(this, pos, target, portType);
  return m_mouseGrabber;
}

MouseGrabber * Graph::getMouseGrabber()
{
  return m_mouseGrabber;
}

void Graph::resetMouseGrabber()
{
  m_mouseGrabber = NULL;
}

void Graph::updateOverlays(float width, float height)
{
  if(m_overlayItem == NULL)
    return;

  QPointF pos = m_overlayPos;
  if(pos.x() < 0.0f) 
  {
    if(m_rightPanel)
      pos.setX(width + pos.x() - m_rightPanel->rect().width());
    else
      pos.setX(width + pos.x());
  }
  else if(m_leftPanel)
  {
    pos.setX(pos.x() + m_leftPanel->rect().width());
  }
  
  if(pos.y() < 0.0) pos.setY(height + pos.y());

  m_overlayItem->setPos(pos);
}

void Graph::setupBackgroundOverlay(QPointF pos, QString filePath)
{
  m_overlayPos = pos;
  m_overlayPixmap = QPixmap(filePath);  
  if(m_overlayItem == NULL)
    m_overlayItem = new QGraphicsPixmapItem(m_overlayPixmap, this);
  else
    m_overlayItem->setPixmap(m_overlayPixmap);
  m_overlayItem->setZValue(2000);
  updateOverlays(rect().width(), rect().height());
}

void Graph::defineHotkey(Qt::Key key, Qt::KeyboardModifier modifiers, QString name)
{
  Hotkey hotkey(key, modifiers);
  std::map<Hotkey, QString>::iterator it = m_hotkeys.find(hotkey);
  if(it == m_hotkeys.end())
    m_hotkeys.insert(std::pair<Hotkey, QString>(hotkey, name));
  else
    it->second = name;
}

bool Graph::pressHotkey(Qt::Key key, Qt::KeyboardModifier modifiers)
{
  Hotkey hotkey(key, modifiers);
  std::map<Hotkey, QString>::iterator it = m_hotkeys.find(hotkey);
  if(it == m_hotkeys.end())
    return false;
  emit hotkeyPressed(it->first.key, it->first.modifiers, it->second);
  return true;
}

bool Graph::releaseHotkey(Qt::Key key, Qt::KeyboardModifier modifiers)
{
  Hotkey hotkey(key, modifiers);
  std::map<Hotkey, QString>::iterator it = m_hotkeys.find(hotkey);
  if(it == m_hotkeys.end())
    return false;
  emit hotkeyReleased(it->first.key, it->first.modifiers, it->second);
  return true;
}

void Graph::onNodeDoubleClicked(FabricUI::GraphView::Node * node, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
  if(m_isEditable)
  {
    if(modifiers.testFlag(Qt::ShiftModifier))
      emit nodeEditRequested(node);
    else
      emit nodeInspectRequested(node);
  }
}

void Graph::onBubbleEditRequested(FabricUI::GraphView::Node * node)
{
  emit bubbleEditRequested(node);
}

void Graph::setGraphContextMenuCallback(Graph::GraphContextMenuCallback callback, void * userData)
{
  m_graphContextMenuCallback = callback;
  m_graphContextMenuCallbackUD = userData;
}

void Graph::setNodeContextMenuCallback(Graph::NodeContextMenuCallback callback, void * userData)
{
  m_nodeContextMenuCallback = callback;
  m_nodeContextMenuCallbackUD = userData;
}

void Graph::setPinContextMenuCallback(Graph::PinContextMenuCallback callback, void * userData)
{
  m_pinContextMenuCallback = callback;
  m_pinContextMenuCallbackUD = userData;
}

void Graph::setConnectionContextMenuCallback(Graph::ConnectionContextMenuCallback callback, void * userData)
{
  m_connectionContextMenuCallback = callback;
  m_connectionContextMenuCallbackUD = userData;
}

void Graph::setPortContextMenuCallback(Graph::PortContextMenuCallback callback, void * userData)
{
  m_portContextMenuCallback = callback;
  m_portContextMenuCallbackUD = userData;
}

void Graph::setSidePanelContextMenuCallback(Graph::SidePanelContextMenuCallback callback, void * userData)
{
  m_sidePanelContextMenuCallback = callback;
  m_sidePanelContextMenuCallbackUD = userData;
}

QMenu* Graph::getGraphContextMenu()
{
  if(!m_graphContextMenuCallback)
    return NULL;
  return (*m_graphContextMenuCallback)(this, m_graphContextMenuCallbackUD);
}

QMenu* Graph::getNodeContextMenu(Node * node)
{
  if(!m_nodeContextMenuCallback)
    return NULL;
  return (*m_nodeContextMenuCallback)(node, m_nodeContextMenuCallbackUD);
}

QMenu* Graph::getPinContextMenu(Pin * pin)
{
  if(!m_pinContextMenuCallback)
    return NULL;
  return (*m_pinContextMenuCallback)(pin, m_pinContextMenuCallbackUD);
}

QMenu* Graph::getConnectionContextMenu(Connection * connection)
{
  if(!m_connectionContextMenuCallback)
    return NULL;
  return (*m_connectionContextMenuCallback)(connection, m_connectionContextMenuCallbackUD);
}

QMenu* Graph::getPortContextMenu(Port * port)
{
  if(!m_portContextMenuCallback)
    return NULL;
  return (*m_portContextMenuCallback)(port, m_portContextMenuCallbackUD);
}

QMenu* Graph::getSidePanelContextMenu(SidePanel * sidePanel)
{
  if(!m_sidePanelContextMenuCallback)
    return NULL;
  return (*m_sidePanelContextMenuCallback)(sidePanel, m_sidePanelContextMenuCallbackUD);
}

void Graph::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QGraphicsWidget::paint(painter, option, widget);
}
