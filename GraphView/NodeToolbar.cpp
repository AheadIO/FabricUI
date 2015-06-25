// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NodeToolbar.h"
#include "Graph.h"
#include "Controller.h"

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtCore/QDebug>

using namespace FabricUI::GraphView;

std::map<QString, QPixmap> NodeToolbar::s_pixmaps;

NodeToolbar::NodeToolbar(Graph * parent, const GraphConfig & config)
: QGraphicsWidget(parent->itemGroup())
{
  m_graph = parent;
  m_config = config;
  m_node = NULL;
  m_enabled = true;
  setZValue(-25);

  setMinimumHeight(m_config.nodeToolbarHeight);
  setMaximumHeight(m_config.nodeToolbarHeight);
  setMinimumWidth(m_config.nodeToolbarBaseWidth);
  setMaximumWidth(m_config.nodeToolbarBaseWidth);

  setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  // layout->addItem(m_mainWidget);
  // setContentsMargins(0, 0, 0, 0);
  // layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
}

NodeToolbar::~NodeToolbar()
{
}

Graph * NodeToolbar::graph()
{
  return m_graph;
}

const Graph * NodeToolbar::graph() const
{
  return m_graph;
}

Node * NodeToolbar::node()
{
  return m_node;
}

const Node * NodeToolbar::node() const
{
  return m_node;
}

void NodeToolbar::disable()
{
  deattach();
  hide();
  m_enabled = false;
}

void NodeToolbar::enable()
{
  m_enabled = true;
  attach(m_node);
}

void NodeToolbar::attach(Node * node)
{
  deattach();
  m_node = node;
  if(m_node == NULL)
    return;

  graph()->controller()->populateNodeToolbar(this, m_node);
  QObject::connect(m_node, SIGNAL(positionChanged(FabricUI::GraphView::Node *, QPointF)), this, SLOT(onNodePositionChanged(FabricUI::GraphView::Node *, QPointF)));
  onNodePositionChanged(m_node, QPointF());

  if(m_enabled)
  {
    show();
    update();
  }
}

void NodeToolbar::deattach(bool disconnectSignal)
{
  if(m_node && disconnectSignal)
  {
    QObject::disconnect(m_node, SIGNAL(positionChanged(FabricUI::GraphView::Node *, QPointF)), this, SLOT(onNodePositionChanged(FabricUI::GraphView::Node *, QPointF)));
  }
  m_node = NULL;
  clearTools();
  hide();
}

void NodeToolbar::addTool(QString name, QString resource, bool performUpdate)
{
  QString filePath = m_config.nodeToolbarIconDir + resource;

  int pos = 0;
  QRegExp rx("\\$\\{([^\\}]+)\\}");
  rx.setMinimal(true);
  while((pos = rx.indexIn(filePath, pos)) != -1)
  {
      QString capture = rx.cap(1);
      const char * envVar = getenv(capture.toAscii());
      if(envVar)
      {
        QString replacement = envVar;
        filePath.replace("${" + capture + "}", replacement);
      }
      pos += rx.matchedLength() + 2;
  }

  Tool tool;
  tool.name = name;
  std::map<QString, QPixmap>::iterator it = s_pixmaps.find(filePath);
  if(it != s_pixmaps.end())
    tool.pixmap = it->second;
  else
  {
    tool.pixmap = QPixmap(filePath);
    s_pixmaps.insert(std::pair<QString, QPixmap>(filePath, tool.pixmap));
  
    if(tool.pixmap.width() == 0)
    {
      printf("Nodetoolbar: Pixmap not found: '%s'\n", filePath.toUtf8().constData());
      return;
    }
  }

  if(tool.pixmap.width() == 0)
    return;


  tool.rotation = 0;
  m_tools.push_back(tool);

  float width = m_config.nodeToolbarBaseWidth;
  width += float(m_tools.size() - 1) * m_config.nodeToolbarSeparator;
  for(size_t i=0;i<m_tools.size();i++)
    width += m_tools[i].pixmap.width();
  setMinimumWidth(width);
  setMaximumWidth(width);
  if(performUpdate)
    update();
}

void NodeToolbar::clearTools()
{
  m_tools.clear();
  setMinimumWidth(m_config.nodeToolbarBaseWidth);
  setMaximumWidth(m_config.nodeToolbarBaseWidth);
  update();
}

int NodeToolbar::toolRotation(QString name) const
{
  for(size_t i=0;i<m_tools.size();i++)
  {
    if(m_tools[i].name == name)
      return m_tools[i].rotation;
  }
  return -1;
}

void NodeToolbar::setToolRotation(QString name, int rotation)
{
  for(size_t i=0;i<m_tools.size();i++)
  {
    if(m_tools[i].name == name)
    {
      m_tools[i].rotation = rotation % 4;
      update();
      return;
    }
  }
}

void NodeToolbar::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(event->button() == Qt::LeftButton && m_node)
  {
    for(size_t  i=0;i<m_tools.size();i++)
    {
      if(toolRect(i).contains(event->pos()))
      {
        emit toolTriggered(m_node, m_tools[i].name.toUtf8().constData());
        break;
      }
    }
    event->accept();
  }
  else
    QGraphicsWidget::mousePressEvent(event);
}

void NodeToolbar::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  if(m_node && m_tools.size() > 0 && m_enabled)
  {
    QPen standardPen = m_config.nodeToolbarPen;
    QRectF rect = windowFrameRect();
    rect.setHeight(m_config.nodeToolbarHeight);

    float radiusW = 150.0f * m_config.nodeCornerRadius / rect.width();
    float radiusH = 150.0f * m_config.nodeCornerRadius / rect.height();
    float radiusHH = 150.0f * m_config.nodeCornerRadius / (rect.height() + radiusH);

    painter->setBrush(m_config.nodeToolbarColor);

    QPainterPath rounded_rect;
    rounded_rect.addRoundRect(rect.adjusted(0, 0, 0, radiusH * 0.1), radiusW, radiusHH);

    // fill everything
    painter->fillPath(rounded_rect, painter->brush());     

    // draw the outline
    painter->strokePath(rounded_rect, standardPen);

    QPointF offset(m_config.nodeToolbarBaseWidth*0.5f, m_config.nodeToolbarBaseWidth*0.5f);
    for(size_t i=0;i<m_tools.size();i++)
    {
      QPixmap pixmap = m_tools[i].pixmap;
      if(m_tools[i].rotation > 0)
      {
        pixmap = pixmap.transformed(QTransform().rotate(90.0f * float(m_tools[i].rotation)));
      }
      painter->drawPixmap(offset, pixmap);
      offset += QPointF(m_tools[i].pixmap.width() + m_config.nodeToolbarSeparator, 0.0f);
    }
  }
  QGraphicsWidget::paint(painter, option, widget);
}

void NodeToolbar::onNodePositionChanged(FabricUI::GraphView::Node * node, QPointF pos)
{
  QPointF p = node->topLeftGraphPos();// + node->windowFrameRect().topRight();
  p -= QPointF(minimumWidth(), minimumHeight());
  p += QPointF(m_node->boundingRect().width() - m_config.nodeWidthReduction * 0.5f - 3.0f - m_config.nodeCornerRadius * 0.5, 0.0f);

  setTransform(QTransform::fromTranslate(p.x(), p.y()), false);

  if(node->size().width() == 0)
    deattach();
}

QRectF NodeToolbar::toolRect(int index) const
{
  QPointF offset(m_config.nodeToolbarBaseWidth*0.5f, m_config.nodeToolbarBaseWidth*0.5f);
  for(size_t i=0;i<m_tools.size();i++)
  {
    if(i == index)
      return QRectF(offset.x(), offset.y(), m_tools[i].pixmap.width(), m_tools[i].pixmap.height());
    offset += QPointF(m_tools[i].pixmap.width() + m_config.nodeToolbarSeparator, 0.0f);
  }
  return QRectF();
}
