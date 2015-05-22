// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Node.h"
#include "NodeToolbar.h"
#include "NodeRectangle.h"
#include "Graph.h"

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtCore/QDebug>

using namespace FabricUI::GraphView;

Node::Node(Graph * parent, QString path, QString label, QColor color, QColor labelColor)
: QGraphicsWidget(parent->itemGroup())
{
  m_graph = parent;
  m_path = path;
  m_labelCaption = label;
  if(m_labelCaption.length() == 0)
    m_labelCaption = name();
  else
  {
    QStringList parts = m_labelCaption.split(m_graph->config().pathSep);
    m_labelCaption = parts[parts.count()-1];
  }

  m_defaultPen = m_graph->config().nodeDefaultPen;
  m_selectedPen = m_graph->config().nodeSelectedPen;
  m_errorPen = m_graph->config().nodeErrorPen;
  m_cornerRadius = m_graph->config().nodeCornerRadius;
  m_collapsedState = CollapseState_Expanded;

  if(color.isValid())
    setColor(color);
  else
    setColor(m_graph->config().nodeDefaultColor);
  if(labelColor.isValid())
    setLabelColor(labelColor);
  else
    setLabelColor(m_graph->config().nodeDefaultLabelColor);

  float contentMargins = m_graph->config().nodeContentMargins;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  m_mainWidget = new NodeRectangle(this);
  m_mainWidget->setMinimumWidth(m_graph->config().nodeMinWidth);
  m_mainWidget->setMinimumHeight(m_graph->config().nodeMinHeight);
  m_mainWidget->setSizePolicy(
    QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding)
    );

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->addItem(m_mainWidget);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  layout = new QGraphicsLinearLayout();
  layout->setContentsMargins(contentMargins, contentMargins, contentMargins, contentMargins);
  layout->setSpacing(1);
  layout->setOrientation(Qt::Vertical);
  m_mainWidget->setLayout(layout);

  m_header = new NodeHeader(this, title());
  layout->addItem(m_header);
  layout->setAlignment(m_header, Qt::AlignHCenter | Qt::AlignTop);

  m_pinsWidget = new QGraphicsWidget(m_mainWidget);
  layout->addItem(m_pinsWidget);
  layout->setAlignment(m_pinsWidget, Qt::AlignHCenter | Qt::AlignVCenter);

  m_pinsLayout = new QGraphicsLinearLayout();
  m_pinsLayout->setOrientation(Qt::Vertical);
  m_pinsLayout->setContentsMargins(0, m_graph->config().nodeSpaceAbovePorts, 0, m_graph->config().nodeSpaceBelowPorts);
  m_pinsLayout->setSpacing(m_graph->config().nodePinSpacing);
  m_pinsWidget->setLayout(m_pinsLayout);

  m_selected = false;
  m_dragging = 0;

  // setup the drop shadow
  if(m_graph->config().nodeShadowEnabled)
  {
    QGraphicsDropShadowEffect * shadow = new QGraphicsDropShadowEffect(m_mainWidget);
    shadow->setColor(m_graph->config().nodeShadowColor);
    shadow->setOffset(m_graph->config().nodeShadowOffset);
    shadow->setBlurRadius(m_graph->config().nodeShadowBlurRadius);
    m_mainWidget->setGraphicsEffect(shadow);
  }

  // setup caching
  m_cache = new CachingEffect(NULL);
  this->setGraphicsEffect(m_cache);

  setAcceptHoverEvents(true);
}

Node::~Node()
{
  if(m_cache)
  {
    delete(m_cache);
    m_cache = NULL;
  }
}

Graph * Node::graph()
{
  return m_graph;
}

const Graph * Node::graph() const
{
  return m_graph;
}

NodeHeader * Node::header()
{
  return m_header;
}

const NodeHeader * Node::header() const
{
  return m_header;
}

QString Node::path() const
{
  return m_path;
}

QString Node::name() const
{
  QStringList parts = m_path.split(m_graph->config().pathSep);
  return parts[parts.count()-1];
}

QString Node::title() const
{
  return m_labelCaption;
}

void Node::setTitle(QString t)
{
  m_labelCaption = t;
  m_header->setTitle(t);
}

QString Node::preset() const
{
  return m_preset;
}

QColor Node::color() const
{
  return m_colorA;
}

void Node::setColor(QColor col)
{
  setColorAsGradient(col, col);
}

void Node::setColorAsGradient(QColor a, QColor b)
{
  m_colorA = a;
  m_colorB = b;
  if(m_graph->config().nodeDefaultPenUsesNodeColor)
    m_defaultPen.setBrush(m_colorB.darker());
}

QColor Node::labelColor() const
{
  return m_labelColor;
}

void Node::setLabelColor(QColor col)
{
  m_labelColor = col;
}

QPen Node::defaultPen() const
{
  return m_defaultPen;
}

QPen Node::selectedPen() const
{
  return m_selectedPen;
}

QRectF Node::boundingRect() const
{
  QRectF rect = QGraphicsWidget::boundingRect();
  if(m_graph->config().nodeShadowEnabled)
  {
    rect.setWidth(rect.width() + m_graph->config().nodeShadowOffset.x() * 2.0);
    rect.setHeight(rect.height() + m_graph->config().nodeShadowOffset.y() * 2.0);
  }
  return rect;
}

bool Node::selected() const
{
  return m_selected;
}

Node::CollapseState Node::collapsedState() const
{
  return m_collapsedState;
}

void Node::setCollapsedState(Node::CollapseState state)
{
  m_collapsedState = state;
  emit collapsedStateChanged(this, m_collapsedState);
  updatePinLayout();
  update();
}

void Node::toggleCollapsedState()
{
  setCollapsedState(CollapseState((int(m_collapsedState) + 1) % int(CollapseState_NumStates)));
}

void Node::setPreset(QString p)
{
  m_preset = p;
}

void Node::setSelected(bool state, bool quiet)
{
  if(state == m_selected)
    return;
  m_selected = state;
  if(m_header)
    m_header->setHighlighted(m_selected);
  if(!quiet)
  {
    emit selectionChanged(this, m_selected);
    if(m_selected)
      emit m_graph->nodeSelected(this);
    else
      emit m_graph->nodeDeselected(this);
  }
  update();
}

QString Node::error() const
{
  return m_errorText;
}

bool Node::hasError() const
{
  return m_errorText.length() > 0;
}

void Node::setError(QString text)
{
  m_errorText = text;
  setToolTip(text);
  update();
}

void Node::clearError()
{
  setError("");
}

QPointF Node::graphPos() const
{
  QTransform xfo = transform();
  QSizeF scale = size();
  QPointF pos(xfo.dx()+(scale.width()*0.5), xfo.dy()+(scale.height()*0.5));
  return pos;
}

void Node::setGraphPos(QPointF pos, bool quiet)
{
  QSizeF scale = size();
  setTransform(QTransform::fromTranslate(pos.x()-(scale.width()*0.5), pos.y()-(scale.height()*0.5)), false);
  if(!quiet)
  {
    emit positionChanged(this, pos);
    emit m_graph->nodeMoved(this, pos);
  }
}

QPointF Node::topLeftGraphPos() const
{
  QTransform xfo = transform();
  QPointF pos(xfo.dx(), xfo.dy());
  return pos;
}

void Node::setTopLeftGraphPos(QPointF pos, bool quiet)
{
  setTransform(QTransform::fromTranslate(pos.x(), pos.y()), false);
  if(!quiet)
  {
    emit positionChanged(this, graphPos());
    emit m_graph->nodeMoved(this, graphPos());
  }
}

QPointF Node::topLeftToCentralPos(QPointF pos)
{
  QSizeF scale = size();
  return QPointF(pos.x() + scale.width() * 0.5, pos.y() + scale.height() * 0.5);
}

QPointF Node::centralPosToTopLeftPos(QPointF pos)
{
  QSizeF scale = size();
  return QPointF(pos.x() - scale.width() * 0.5, pos.y() - scale.height() * 0.5);
}

Pin * Node::addPin(Pin * pin, bool quiet)
{
  // todo: we need a method to update the layout based on the collapsed state.....
  for(size_t i=0;i<m_pins.size();i++)
  {
    if(m_pins[i] == pin)
      return NULL;
    if(m_pins[i]->name() == pin->name())
      return NULL;
  }

  pin->setIndex((int)m_pins.size());
  m_pins.push_back(pin);
  if(!quiet)
    emit pinAdded(this, pin);

  updatePinLayout();
  return pin;
}

bool Node::removePin(Pin * pin, bool quiet)
{
  size_t index = m_pins.size();
  for(size_t i=0;i<m_pins.size();i++)
  {
    if(m_pins[i] == pin)
    {
      index = i;
      break;
    }
  }
  if(index == m_pins.size())
    return false;

  m_pins.erase(m_pins.begin() + index);
  updatePinLayout();
  if(!quiet)
    emit pinRemoved(this, pin);

  scene()->removeItem(pin);
  pin->deleteLater();
  delete(pin);

  update();

  return true;
}

std::vector<Node*> Node::upStreamNodes(bool sortForPins, std::vector<Node*> rootNodes)
{
  int maxCol = 0;

  std::map<Node*, Node*> visitedNodes;
  std::vector<Node*> nodes;

  if(rootNodes.size() == 0)
  {
    visitedNodes.insert(std::pair<Node*, Node*>(this, this));
    nodes.push_back(this);
  }
  else
  {
    for(size_t i=0;i<rootNodes.size();i++)
    {
      visitedNodes.insert(std::pair<Node*, Node*>(rootNodes[i], rootNodes[i]));
      nodes.push_back(rootNodes[i]);
    }
  }

  for(size_t i=0;i<nodes.size();i++)
  {
    if(nodes[i]->col() == -1)
      nodes[i]->setCol(0);
  }

  std::vector<Connection*> connections = graph()->connections();
  for(size_t i=0;i<nodes.size();i++)
  {
    for(size_t k=0;k<nodes[i]->m_pins.size();k++)
    {
      for(size_t j=0;j<connections.size();j++)
      {
        ConnectionTarget * dst = connections[j]->dst();
        ConnectionTarget * src = connections[j]->src();
        if(!dst || !src)
          continue;
        if(dst->targetType() != TargetType_Pin || src->targetType() != TargetType_Pin)
          continue;

        Pin * dstPin = (Pin*)dst;
        Pin * srcPin = (Pin*)src;
        Node * dstNode = dstPin->node();
        Node * srcNode = srcPin->node();

        if(dstNode != nodes[i])
          continue;
        if(visitedNodes.find(srcNode) != visitedNodes.end())
          continue;
        if(sortForPins && dstPin != nodes[i]->m_pins[k])
          continue;

        if(srcNode->col() < nodes[i]->col() + 1)
          srcNode->setCol(nodes[i]->col() + 1);
        if(srcNode->col() > maxCol)
          maxCol = srcNode->col();

        visitedNodes.insert(std::pair<Node*, Node*>(srcNode, srcNode));
        nodes.push_back(srcNode);
      }

      if(!sortForPins)
        break;
    }
  }

  std::vector<int> rows(maxCol+1);
  for(int i=0; i<maxCol; i++)
  {
    rows[i] = 0;
  }

  for(size_t i=0;i<nodes.size();i++)
  {
    nodes[i]->setRow(rows[nodes[i]->col()]);
    rows[nodes[i]->col()]++;
  }

  return nodes;
}

int Node::row() const
{
  return m_row;
}

void Node::setRow(int i)
{
  m_row = i;
}

int Node::col() const
{
  return m_col;
}

void Node::setCol(int i)
{
  m_col = i;
}

unsigned int Node::pinCount() const
{
  return m_pins.size();
}

Pin * Node::pin(unsigned int index)
{
  return m_pins[index];
}

Pin * Node::pin(QString name)
{
  for(unsigned int i=0;i<m_pins.size();i++)
  {
    if(m_pins[i]->name() == name)
      return m_pins[i];
  }
  return NULL;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(event->modifiers().testFlag(Qt::AltModifier))
    return QGraphicsWidget::mousePressEvent(event);

  if(event->button() == Qt::LeftButton || event->button() == DFG_QT_MIDDLE_MOUSE)
  {
    bool clearSelection = true;
    if(event->button() == DFG_QT_MIDDLE_MOUSE)
    {
      std::vector<Node*> nodes = upStreamNodes();

      if(!event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier))
      {
        m_graph->controller()->clearSelection();
        clearSelection = false;
      }

      for(size_t i=0;i<nodes.size();i++)
        m_graph->controller()->selectNode(nodes[i], true);
    }

    m_dragging = 1;
    m_lastDragPoint = mapToItem(m_graph->itemGroup(), event->pos());
    event->accept();

    if(!selected())
    {
      m_graph->controller()->beginInteraction();

      if(clearSelection && !event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier))
        m_graph->controller()->clearSelection();
      m_graph->controller()->selectNode(this, true);

      m_graph->controller()->endInteraction();
    }
    else if(event->modifiers().testFlag(Qt::ControlModifier))
    {
      m_graph->controller()->selectNode(this, false);
    }
  }
  else if(event->button() == Qt::RightButton)
  {
    QMenu * menu = graph()->getNodeContextMenu(this);
    if(menu)
    {
      menu->exec(QCursor::pos());
      menu->deleteLater();
    }
    else
      QGraphicsWidget::mousePressEvent(event);
  }
  else
    QGraphicsWidget::mousePressEvent(event);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_dragging > 0)
  {
    QPointF pos = mapToItem(m_graph->itemGroup(), event->pos());
    QPointF delta = pos - m_lastDragPoint;
    m_lastDragPoint = pos;
    m_dragging = 2;

    m_graph->controller()->beginInteraction();

    std::vector<Node *> nodes = m_graph->selectedNodes();
    for(size_t i=0;i<nodes.size();i++)
      m_graph->controller()->moveNode(nodes[i], nodes[i]->graphPos() + delta);

    m_graph->controller()->endInteraction();
  }
  else
    QGraphicsWidget::mouseMoveEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_dragging == 2)
  {
    if(!selected())
      emit positionChanged(this, graphPos());
    else
    {
      m_graph->controller()->beginInteraction();

      std::vector<Node *> nodes = m_graph->selectedNodes();
      for(size_t i=0;i<nodes.size();i++)
        m_graph->controller()->moveNode(nodes[i], nodes[i]->graphPos());

      m_graph->controller()->endInteraction();
    }
    m_dragging = false;
  }
  else
    QGraphicsWidget::mouseReleaseEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  emit doubleClicked(this);
  QGraphicsWidget::mouseDoubleClickEvent(event);
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  graph()->nodeToolbar()->attach(this);
  event->accept();
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
}

QGraphicsWidget * Node::mainWidget()
{
  return m_mainWidget;
}

QGraphicsWidget * Node::pinsWidget()
{
  return m_pinsWidget;
}

void Node::onConnectionsChanged()
{
  if(m_collapsedState == CollapseState_OnlyConnections)
  {
    updatePinLayout();
  }
}

void Node::updatePinLayout()
{
  int count = m_pinsLayout->count();
  if(count > 0)
  {
    for(int i=count-1;i>=0;i--)
      m_pinsLayout->removeAt(i);
    m_pinsLayout->invalidate();
  }

  for(size_t i=0;i<m_pins.size();i++)
  {
    bool showPin = m_collapsedState == CollapseState_Expanded;
    if(!showPin && m_collapsedState == CollapseState_OnlyConnections)
      showPin = m_pins[i]->isConnected();
    m_pins[i]->setDrawState(showPin);
    if(showPin)
    {
      m_pinsLayout->addItem(m_pins[i]);
      m_pinsLayout->setAlignment(m_pins[i], Qt::AlignLeft | Qt::AlignTop);
    }
  }

  if(m_pinsLayout->count() == 0 && m_pins.size() > 0)
  {
    m_pinsLayout->addItem(m_pins[0]);
    m_pinsLayout->setAlignment(m_pins[0], Qt::AlignLeft | Qt::AlignTop);
  }
}
