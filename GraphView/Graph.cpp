// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Graph.h"
#include "Exception.h"

using namespace FabricUI::GraphView;

Graph::Graph(QGraphicsItem * parent, const GraphConfig & config, GraphFactory * factory)
: QGraphicsWidget(parent)
{
  m_config = config;
  m_factory = factory;
  m_path = "";

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  setMinimumSize(400, 400);

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
  m_nodeToolbar = NULL;

  m_constructed = false;
  reset("graph", false);
  m_constructed = true;
}

void Graph::reset(QString path, bool createSidePanels)
{
  m_path = path;

  if(m_nodeToolbar)
    m_nodeToolbar->deattach(false);

  prepareGeometryChange();


  if(m_constructed)
  {
    for(size_t i=0;i<m_connections.size();i++)
    {
      scene()->removeItem(m_connections[i]);
      delete(m_connections[i]);
    }
    m_connections.clear();

    for(size_t i=0;i<m_nodes.size();i++)
    {
      scene()->removeItem(m_nodes[i]);
      delete(m_nodes[i]);
    }
    m_nodes.clear();
    m_nodeMap.clear();

    if(m_mouseGrabber)
    {
      scene()->removeItem(m_mouseGrabber);
      delete(m_mouseGrabber);
      m_mouseGrabber = NULL;
    }
    if(m_nodeToolbar)
    {
      scene()->removeItem(m_nodeToolbar);
      delete(m_nodeToolbar);
      m_nodeToolbar = NULL;
    }
    if(m_mainPanel)
    {
      scene()->removeItem(m_mainPanel);
      delete(m_mainPanel);
      m_mainPanel = NULL;
    }
    if(m_leftPanel)
    {
      scene()->removeItem(m_leftPanel);
      delete(m_leftPanel);
      m_leftPanel = NULL;
    }
    if(m_rightPanel)
    {
      scene()->removeItem(m_rightPanel);
      delete(m_rightPanel);
      m_rightPanel = NULL;
    }
  }

  m_mainPanel = new MainPanel(this);

  if(createSidePanels)
  {
    m_leftPanel = new SidePanel(this, path, PortType_Output);
    m_rightPanel = new SidePanel(this, path, PortType_Input);
  }

  setContentsMargins(0, 0, 0, 0);
  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  if(m_leftPanel)
  {
    layout->addItem(m_leftPanel);
    QObject::connect(m_leftPanel, SIGNAL(doubleClicked(FabricUI::GraphView::SidePanel*)), 
      this, SLOT(onSidePanelDoubleClicked(FabricUI::GraphView::SidePanel*)));
  }
  layout->addItem(m_mainPanel);
  if(m_rightPanel)
  {
    layout->addItem(m_rightPanel);
    QObject::connect(m_rightPanel, SIGNAL(doubleClicked(FabricUI::GraphView::SidePanel*)), 
      this, SLOT(onSidePanelDoubleClicked(FabricUI::GraphView::SidePanel*)));
  }

  m_nodeToolbar = new NodeToolbar(this, m_config);
  m_nodeToolbar->hide();
  if(m_controller)
  {
    QObject::connect(m_nodeToolbar, SIGNAL(toolTriggered(FabricUI::GraphView::Node *, QString)), 
      m_controller, SLOT(nodeToolTriggered(FabricUI::GraphView::Node *, QString)));
  }
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
  if(!m_controller && c && m_nodeToolbar)
  {
    QObject::connect(m_nodeToolbar, SIGNAL(toolTriggered(FabricUI::GraphView::Node *, QString)), 
      c, SLOT(nodeToolTriggered(FabricUI::GraphView::Node *, QString)));
  }
  m_controller = c;
}

QString Graph::path() const
{
  return m_path;
}

void Graph::setPath(QString path)
{
  m_path = path;
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

NodeToolbar * Graph::nodeToolbar()
{
  return m_nodeToolbar;
}

Node * Graph::addNode(Node * node, bool quiet)
{
  QString key = node->path();
  if(m_nodeMap.find(key) != m_nodeMap.end())
    return NULL;

  m_nodeMap.insert(std::pair<QString, size_t>(key, m_nodes.size()));
  m_nodes.push_back(node);

  QObject::connect(node, SIGNAL(doubleClicked(FabricUI::GraphView::Node*)), this, SLOT(onNodeDoubleClicked(FabricUI::GraphView::Node*)));

  if(m_nodeToolbar)
    m_nodeToolbar->attach(node);

  if(!quiet)
    emit nodeAdded(node);

  return node;
}

Node * Graph::addNodeFromPreset(QString path, QString preset, bool quiet)
{
  if(!m_factory)
  {
    if(preset == "")
    {
      return addNode(new Node(this, path), quiet);
    }
    return NULL;
  }
  Node * node = m_factory->constructNodeFromPreset(this, path, preset);
  if(!node)
    return NULL;
  node->setPreset(preset);
  return addNode(node, quiet);
}

bool Graph::removeNode(Node * node, bool quiet)
{
  QString key = node->path();
  std::map<QString, size_t>::iterator it = m_nodeMap.find(key);
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
      controller()->removeConnection(con);
  }

  if(m_nodeToolbar->node() == node)
    m_nodeToolbar->deattach();

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

Node * Graph::node(QString name) const
{
  QString key = path();
  if(key.length() > 0)
    key += m_config.pathSep;
  key += name;
  std::map<QString, size_t>::const_iterator it = m_nodeMap.find(key);
  if(it == m_nodeMap.end())
    return NULL;
  return m_nodes[it->second];
}

Node * Graph::nodeFromPath(QString path) const
{
  std::map<QString, size_t>::const_iterator it = m_nodeMap.find(path);
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

Port * Graph::port(QString name) const
{
  if(!hasSidePanels())
    return NULL;

  for(unsigned int i=0;i<m_leftPanel->portCount();i++)
  {
    if(m_leftPanel->port(i)->name() == name)
      return m_leftPanel->port(i);
  }
  for(unsigned int i=0;i<m_rightPanel->portCount();i++)
  {
    if(m_rightPanel->port(i)->name() == name)
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
          if(((Port*)m_connections[i]->src())->path() == ((Port*)m_connections[i]->dst())->path())
            continue;
        }

        if(!controller()->removeConnection(m_connections[i]))
          return NULL;
        break;
      }
    }
  }

  prepareGeometryChange();
  controller()->beginInteraction();

  Connection * connection;
  if(m_factory)
    connection = m_factory->constructConnection(this, src, dst);
  else
    connection = new Connection(this, src, dst);
  m_connections.push_back(connection);

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

  m_connections.erase(m_connections.begin() + index);
  if(!quiet)
    emit connectionRemoved(connection);

  prepareGeometryChange();
  connection->invalidate();
  scene()->removeItem(connection);
  delete(connection);

  controller()->endInteraction();

  return true;
}


MouseGrabber * Graph::constructMouseGrabber(QPointF pos, ConnectionTarget * target, PortType portType)
{
  m_mouseGrabber = new MouseGrabber(this, pos, target, portType);
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

void Graph::onNodeDoubleClicked(FabricUI::GraphView::Node * node)
{
  emit nodeDoubleClicked(node);
}

void Graph::onSidePanelDoubleClicked(FabricUI::GraphView::SidePanel * panel)
{
  emit sidePanelDoubleClicked(panel);
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

QString Graph::getUniquePath(QString path) const
{
  QString resolvedPath;
  unsigned int suffixIndex = 0;
  QString prefix;
  QString middle;
  QString suffix;

  QStringList parts = path.split(config().pathSep);
  for(int i=0;i<parts.count()-1;i++)
  {
    if(i>0)
      prefix += config().pathSep;
    prefix += parts[i].remove(" ");
  }
  if(prefix.length() > 0)
    prefix += config().pathSep;

  middle = parts[parts.count()-1].remove(" ");

  bool found = true;
  while(found)
  {
    found = false;
    suffix = QString::number(++suffixIndex);
    resolvedPath = prefix + middle + suffix;
    for(size_t i=0;i<m_nodes.size();i++)
    {
      if(m_nodes[i]->path() == resolvedPath)
      {
        found = true;
        break;
      }
    }
  }

  return resolvedPath;
}
