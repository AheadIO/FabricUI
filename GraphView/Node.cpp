// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/NodeLabel.h>
#include <FabricUI/GraphView/NodeRectangle.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/Graph.h>

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtCore/QDebug>

using namespace FabricUI::GraphView;

Node::Node(
  Graph * parent,
  FTL::CStrRef name,
  FTL::CStrRef title,
  QColor color,
  QColor titleColor,
  bool isBackDropNode 
  )
  : QGraphicsWidget( parent->itemGroup() )
  , m_isBackDropNode(isBackDropNode)
  , m_graph( parent )
  , m_name( name )
  , m_title( title )
  , m_bubble( NULL )
  , m_header( NULL )
  , m_mainWidget( NULL )
{
  m_cache = NULL;
  m_defaultPen = m_graph->config().nodeDefaultPen;
  m_selectedPen = m_graph->config().nodeSelectedPen;
  m_errorPen = m_graph->config().nodeErrorPen;
  m_cornerRadius = m_graph->config().nodeCornerRadius;
  m_collapsedState = CollapseState_Expanded;
  m_col = 0;
  m_alwaysShowDaisyChainPorts = false;

  if(color.isValid())
    setColor(color);
  else
    setColor(m_graph->config().nodeDefaultColor);
  if(titleColor.isValid())
    setTitleColor(titleColor);
  else
    setTitleColor(m_graph->config().nodeDefaultLabelColor);

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
  m_mainWidget->setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(contentMargins, 0, contentMargins, 0);
  layout->setSpacing(1);
  layout->setOrientation(Qt::Vertical);
  m_mainWidget->setLayout(layout);

  m_header = new NodeHeader(this, QSTRING_FROM_FTL_UTF8(title) );
  layout->addItem(m_header);
  layout->setAlignment(m_header, Qt::AlignHCenter | Qt::AlignVCenter);

  if(!m_isBackDropNode)
  {
    QObject::connect(m_header, SIGNAL(headerButtonTriggered(FabricUI::GraphView::NodeHeaderButton*)), 
      m_graph->controller(), SLOT(onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton*)));
  }

  QObject::connect(
    m_graph->mainPanel(), SIGNAL(canvasPanChanged(QPointF)),
    this, SLOT(canvasPanned())
    );

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
  m_cache = new CachingEffect(this);
  this->setGraphicsEffect(m_cache);
  
  m_bubble = new GraphView::NodeBubble( graph(), this, graph()->config() );
  m_bubble->hide();

  setAcceptHoverEvents(true);
}

void Node::canvasPanned()
{
  m_mainWidget->update();
}

Node::~Node()
{
  delete m_cache;

  m_bubble->setNode( NULL );
  m_bubble->scene()->removeItem( m_bubble );
  m_bubble->deleteLater();
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

NodeBubble * Node::bubble()
{
  return m_bubble;
}

const NodeBubble * Node::bubble() const
{
  return m_bubble;
}

void Node::setTitle( FTL::CStrRef title )
{
  m_title = title;
  if(m_header)
  {
    std::string titleToSet = m_title + m_titleSuffix;
    m_header->setTitle( QSTRING_FROM_FTL_UTF8(titleToSet) );
    m_header->labelWidget()->setItalic(m_titleSuffix.length() > 0);
  }
}

void Node::setTitleSuffix( FTL::CStrRef titleSuffix )
{
  m_titleSuffix = titleSuffix;
  setTitle(m_title);
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
  if ( m_mainWidget )
    m_mainWidget->update();
}

QColor Node::titleColor() const
{
  return m_titleColor;
}

void Node::setTitleColor(QColor col)
{
  m_titleColor = col;
  if(m_header)
    m_header->setColor(m_titleColor.lighter());
}

QColor Node::fontColor() const
{
  return m_fontColor;
}

void Node::setFontColor(QColor col)
{
  if(m_fontColor == col)
    return;

  m_fontColor = col;
  m_header->labelWidget()->setColor(m_fontColor, m_header->labelWidget()->highlightColor());

  for(size_t i=0;i<m_pins.size();i++)
  {
    m_pins[i]->labelWidget()->setColor(m_fontColor, m_pins[i]->labelWidget()->highlightColor());
  }
}

QPen Node::defaultPen() const
{
  return m_defaultPen;
}

QPen Node::selectedPen() const
{
  return m_selectedPen;
}

QString Node::comment() const
{
  if(m_bubble == NULL)
    return "";
  return m_bubble->text();
}

QRectF Node::boundingRect() const
{
  QRectF rect = QGraphicsWidget::boundingRect();

  float left = rect.left();
  float top = rect.top();
  float width = rect.width();
  float height = rect.height();

  if(m_graph->config().nodeShadowEnabled)
  {
    width += m_graph->config().nodeShadowOffset.x() * 2.0f;
    height += m_graph->config().nodeShadowOffset.y() * 2.0f;
  }

  return QRectF(left, top, width, height);
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
  if(m_collapsedState == state)
    return;
  m_collapsedState = state;
  if(!m_graph->config().nodeHeaderAlwaysShowPins)
    m_header->setCirclesVisible(state != CollapseState_Expanded);
  emit collapsedStateChanged(this, m_collapsedState);
  m_header->setHeaderButtonState("node_collapse", (int)m_collapsedState);
  updatePinLayout();
  update();
}

void Node::toggleCollapsedState()
{
  setCollapsedState(CollapseState((int(m_collapsedState) + 1) % int(CollapseState_NumStates)));
}

void Node::setSelected(bool state, bool quiet)
{
  if(state == m_selected)
    return;
  m_selected = state;
  if(m_header)
  {
    m_header->setHighlighted(m_selected);
    m_header->setNodeButtonsHighlighted(m_selected);
  }
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
  return topLeftToCentralPos(topLeftGraphPos());
}

void Node::setGraphPos(QPointF pos, bool quiet)
{
  setTopLeftGraphPos(centralPosToTopLeftPos(pos));
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

QPointF Node::topLeftToCentralPos(QPointF pos) const
{
  QRectF rect = boundingRect();
  return QPointF(pos.x() + rect.width() * 0.5, pos.y() + rect.height() * 0.5);
}

QPointF Node::centralPosToTopLeftPos(QPointF pos) const
{
  QRectF rect = boundingRect();
  return QPointF(pos.x() - rect.width() * 0.5, pos.y() - rect.height() * 0.5);
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

void Node::reorderPins(QStringList names)
{
  std::vector<Pin*> pins;
  for(int i=0;i<names.length();i++)
  {
    pins.push_back(pin(names[i].toUtf8().constData()));
    pins[i]->setIndex(i);
  }
  m_pins = pins;
  updatePinLayout();
  update();
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

void Node::setAlwaysShowDaisyChainPorts(bool state)
{
  if(m_alwaysShowDaisyChainPorts == state)
    return;
  m_alwaysShowDaisyChainPorts = state;
  updatePinLayout(); 
}

unsigned int Node::pinCount() const
{
  return m_pins.size();
}

Pin * Node::pin(unsigned int index)
{
  return m_pins[index];
}

Pin * Node::pin(FTL::StrRef name)
{
  for(unsigned int i=0;i<m_pins.size();i++)
  {
    if(name == m_pins[i]->name())
      return m_pins[i];
  }
  return NULL;
}

Pin *Node::renamePin( FTL::StrRef oldName, FTL::StrRef newName )
{
  Pin *p = pin( oldName );
  if ( p )
    p->setName( newName );
  return p;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(onMousePress(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mousePressEvent(event);
}

void Node::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
  if(onMouseMove(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mouseMoveEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(onMouseRelease(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mouseReleaseEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  if(onMouseDoubleClicked(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mouseDoubleClickEvent(event);
}

bool Node::onMousePress(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if(modifiers.testFlag(Qt::AltModifier))
    return false;

  if ( button == Qt::LeftButton
    || button == DFG_QT_MIDDLE_MOUSE
    || button == Qt::RightButton )
  {
    m_dragButton = button;
    m_mouseDownPos = scenePos;

    Node * hitNode = this;

    // apparently qt doesn't cast again on right
    // mouse button, so contextual menus are off.
    std::vector<Node *> nodes = graph()->nodes();
    for(size_t i=0;i<nodes.size();i++)
    {
      QPointF pos = nodes[i]->mapFromScene(scenePos);
      if(nodes[i]->rect().contains(pos))
      {
        if(nodes[i]->zValue() < hitNode->zValue())
          continue;
        hitNode = nodes[i];
      }
    }

    bool clearSelection = true;
    if(button == DFG_QT_MIDDLE_MOUSE)
    {
      std::vector<Node*> nodes = hitNode->upStreamNodes();

      if(!modifiers.testFlag(Qt::ControlModifier) && !modifiers.testFlag(Qt::ShiftModifier))
      {
        m_graph->controller()->clearSelection();
        clearSelection = false;
      }

      for(size_t i=0;i<nodes.size();i++)
        m_graph->controller()->selectNode(nodes[i], true);
    }
    else if(button == Qt::RightButton)
    {
      clearSelection = !hitNode->selected();
    }

    m_dragging = button == Qt::RightButton ? 0 : 1;

    if(!hitNode->selected())
    {
      m_graph->controller()->beginInteraction();

      if(clearSelection && !modifiers.testFlag(Qt::ControlModifier) && !modifiers.testFlag(Qt::ShiftModifier))
        m_graph->controller()->clearSelection();
      m_graph->controller()->selectNode(hitNode, true);

      m_graph->controller()->endInteraction();
    }
    else if(modifiers.testFlag(Qt::ControlModifier))
    {
      m_graph->controller()->selectNode(hitNode, false);
    }

    m_nodesToMove = m_graph->selectedNodes();
    if ( !modifiers.testFlag( Qt::ShiftModifier ) )
    {
      std::vector<Node *> additionalNodes;

      for ( std::vector<Node *>::const_iterator it = m_nodesToMove.begin();
        it != m_nodesToMove.end(); ++it )
      {
        Node *node = *it;
        if ( node->isBackDropNode() )
        {
          BackDropNode *backDropNode = static_cast<BackDropNode *>( node );
          backDropNode->appendOverlappingNodes( additionalNodes );
        }
      }

      m_nodesToMove.insert(
        m_nodesToMove.end(),
        additionalNodes.begin(),
        additionalNodes.end()
        );
    }

    if(button == Qt::RightButton)
    {
      QMenu * menu = graph()->getNodeContextMenu(hitNode);
      if(menu)
      {
        menu->exec(QCursor::pos());
        menu->deleteLater();
      }
    }

    return true;
  }

  return false;
}

bool Node::onMouseMove(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if ( m_dragging > 0 )
  {
    m_dragging = 2;

    QPointF delta = scenePos - lastScenePos;
    delta *= 1.0f / graph()->mainPanel()->canvasZoom();

    m_graph->controller()->gvcDoMoveNodes(
      m_nodesToMove,
      delta,
      false // allowUndo
      );

    return true;
  }

  return false;
}

bool Node::onMouseRelease(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if ( m_dragging == 2 )
  {
    if(!selected())
      emit positionChanged(this, graphPos());
    else
    {
      m_graph->controller()->gvcDoMoveNodes(
        m_nodesToMove,
        m_mouseDownPos - lastScenePos,
        false // allowUndo
        );

      m_graph->controller()->gvcDoMoveNodes(
        m_nodesToMove,
        scenePos - m_mouseDownPos,
        true // allowUndo
        );
    }

    m_dragging = 0;
    m_nodesToMove.clear();

    return true;
  }

  return false;
}

bool Node::onMouseDoubleClicked(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if(button == Qt::LeftButton)
  {
    emit doubleClicked(this, button, modifiers);
    return true;
  }
  return false;
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
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

void Node::onBubbleEditRequested(FabricUI::GraphView::NodeBubble * bubble)
{
  emit bubbleEditRequested(this);  
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

  for(size_t i=0;i<m_pins.size();i++)
  {
    m_pins[i]->setDaisyChainCircleVisible(m_alwaysShowDaisyChainPorts);
  }
}

#if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
void Node::updateGeometry()
{
  QGraphicsWidget::updateGeometry();
  emit geometryChanged();
}
#endif
