// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Connection.h>

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsView>

#include <math.h>

using namespace FabricUI::GraphView;

SidePanel::SidePanel(Graph * parent, PortType portType, QColor color)
  : QGraphicsWidget( parent )
  , m_dragTargetIndex( -1 )
{
  m_itemGroup = new SidePanelItemGroup(this);
  m_itemGroup->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
  m_itemGroupScroll = 0.0f;

  const GraphConfig & config = parent->config();

  m_graph = parent;
  m_color = color;
  if(!m_color.isValid())
    m_color = config.sidePanelBackgroundColor;
  m_pen = config.sidePanelPen;
  m_portType = portType;
  m_requiresToSendSignalsForPorts = false;

  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
  setMinimumWidth(config.sidePanelCollapsedWidth);
  setContentsMargins(0, 0, 0, 0);
  setAcceptDrops( true );

  m_proxyPort = new ProxyPort(this, m_portType);

  QObject::connect(m_itemGroup, SIGNAL(resized()), this, SLOT(onItemGroupResized()));

  resetLayout();
}

Graph * SidePanel::graph()
{
  return m_graph;
}

const Graph * SidePanel::graph() const
{
  return m_graph;
}

QGraphicsWidget * SidePanel::itemGroup()
{
  return m_itemGroup;
}

const QGraphicsWidget * SidePanel::itemGroup() const
{
  return m_itemGroup;
}

QColor SidePanel::color() const
{
  return m_color;
}

PortType SidePanel::portType() const
{
  return m_portType;
}

Port * SidePanel::addPort(Port * port)
{
  // std::vector<Port*> ports = graph()->ports();
  std::vector<Port*> ports = m_ports;
  for(size_t i=0;i<ports.size();i++)
  {
    if(ports[i]->name() == port->name())
      return NULL;
  }

  port->setIndex(m_ports.size());
  m_ports.push_back(port);

  resetLayout();
  updateItemGroupScroll();

  return port;
}

bool SidePanel::removePort(Port * port)
{
  size_t index = m_ports.size();
  for(size_t i=0;i<m_ports.size();i++)
  {
    if(m_ports[i] == port)
    {
      index = i;
      break;
    }
  }
  if(index == m_ports.size())
    return false;

  m_ports.erase(m_ports.begin() + index);

  for(size_t i=0;i<m_ports.size();i++)
    m_ports[i]->setIndex(i);

  scene()->removeItem(port);
  delete(port);

  resetLayout();
  updateItemGroupScroll();

  return true;
}

void SidePanel::reorderPorts(QStringList names)
{
  std::vector<Port*> ports;
  for(int i=0;i<names.length();i++)
  {
    ports.push_back(port(names[i].toUtf8().constData()));
    ports[i]->setIndex(i);
  }

  m_ports = ports;
  resetLayout();
}

unsigned int SidePanel::portCount() const
{
  return m_ports.size();
}

Port * SidePanel::port(unsigned int index)
{
  return m_ports[index];
}

Port * SidePanel::port(FTL::StrRef name)
{
  for(size_t i=0;i<m_ports.size();i++)
  {
    if(m_ports[i]->name() == name)
      return m_ports[i];
  }
  return NULL;
}

void SidePanel::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(event->button() == Qt::RightButton)
  {
    QMenu * menu = m_graph->getSidePanelContextMenu(this);
    if(menu)
    {
      menu->exec(QCursor::pos());
      menu->deleteLater();
    }
  }

  if(event->button() == Qt::LeftButton)
  {
    event->accept();
    return;
  }

  QGraphicsWidget::mousePressEvent(event);
}

void SidePanel::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsWidget::mouseMoveEvent(event);
}

void SidePanel::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsWidget::mouseReleaseEvent(event);
}

void SidePanel::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  emit doubleClicked(this);
  QGraphicsWidget::mouseDoubleClickEvent(event);
}

void SidePanel::wheelEvent(QGraphicsSceneWheelEvent * event)
{
  QGraphicsWidget::wheelEvent(event);
  scroll(event->delta() * 0.1);
} 

void SidePanel::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QRectF rect = windowFrameRect();

  painter->fillRect(rect, m_graph->config().mainPanelBackgroundColor);  

  if(m_portType == PortType_Input)
    rect.adjust(m_graph->config().sidePanelWidthReduction, 0, 0, 0);
  else
    rect.adjust(0, 0, -m_graph->config().sidePanelWidthReduction, 0);

  painter->setBrush(m_color);
  painter->setPen(m_pen);
  painter->drawRect(rect);  

  QGraphicsWidget::paint(painter, option, widget);

  if(m_requiresToSendSignalsForPorts)
  {
    for(size_t i=0;i<m_ports.size();i++)
    {
      emit m_ports[i]->positionChanged();
    }
    m_requiresToSendSignalsForPorts = false;
  }

  if ( m_dragTargetIndex != -1 )
  {
    QList<QGraphicsItem *> portLabels = m_itemGroup->childItems();
    qreal y;
    if ( m_dragTargetIndex < portLabels.size() )
    {
      QGraphicsItem *portLabel = portLabels[m_dragTargetIndex];
      y = m_itemGroup->mapToParent( portLabel->pos() ).y();
    }
    else
    {
      QGraphicsItem *portLabel = portLabels.back();
      y = m_itemGroup->mapToParent( portLabel->pos() ).y()
        + portLabel->boundingRect().height() + 8;
    }
    y -= 4;
    painter->setPen( QPen( Qt::white ) );
    painter->drawLine(
      QPointF( rect.left(), y ), QPointF( rect.right(), y )
      );
  }
}

void SidePanel::onItemGroupResized()
{
  setMinimumWidth(m_itemGroup->size().width());
  setMaximumWidth(m_itemGroup->size().width());
}

void SidePanel::resizeEvent(QGraphicsSceneResizeEvent * event)
{
  QGraphicsWidget::resizeEvent(event);
  m_itemGroup->resize(event->newSize().width(), m_itemGroup->size().height());
  setMinimumWidth(m_itemGroup->size().width());
  updateItemGroupScroll(event->newSize().height());
}

void SidePanel::resetLayout()
{
  const GraphConfig & config = graph()->config();
  float contentMargins = config.sidePanelContentMargins;

  QGraphicsLinearLayout *portsLayout = new QGraphicsLinearLayout();
  portsLayout->setContentsMargins(contentMargins, contentMargins, contentMargins, contentMargins);
  portsLayout->setSpacing(config.sidePanelSpacing);
  portsLayout->setOrientation(Qt::Vertical);

  portsLayout->addItem(m_proxyPort);
  portsLayout->setAlignment(m_proxyPort, Qt::AlignRight | Qt::AlignTop);
  portsLayout->setItemSpacing(0, 20);

  for(size_t i=0;i<m_ports.size();i++)
  {
    portsLayout->addItem(m_ports[i]);
    portsLayout->setAlignment(m_ports[i], Qt::AlignRight | Qt::AlignTop);
  }
  portsLayout->addStretch(2);

  m_itemGroup->setLayout(portsLayout);

  m_requiresToSendSignalsForPorts = true;
}

void SidePanel::scroll(float delta)
{
  m_itemGroupScroll += delta;
  updateItemGroupScroll();
}

void SidePanel::updateItemGroupScroll(float height)
{
  if(height == 0.0f)
  {
    height = m_graph->rect().height();

    if(scene())
    {
      if(scene()->views().count() > 0)
      {
        QGraphicsView * view = scene()->views()[0];
        height = (float)view->size().height();
      }
    }
  }

  if(m_itemGroup->size().height() < height)
  {
    if(graph()->config().portsCentered)
      m_itemGroupScroll = 0.5f * (height - m_itemGroup->size().height());
    else
      m_itemGroupScroll = 0.0f;
  }
  else
  {
    float maxScroll =  height - m_itemGroup->size().height();
    maxScroll -= 50.0f;
    if(m_itemGroupScroll < maxScroll)
      m_itemGroupScroll = maxScroll;
    if(m_itemGroupScroll > 0.0f)
      m_itemGroupScroll = 0.0;
  }

  m_itemGroup->setTransform(QTransform::fromTranslate(0, m_itemGroupScroll), false);

  // [Julien]
  // Update the image overlay when panels are resized.
  // We do it here because the image position depends on the panels position 
  graph()->updateOverlays(graph()->rect().width(), graph()->rect().height());

  emit scrolled();
}

void SidePanel::dragMoveEvent( QGraphicsSceneDragDropEvent *event )
{
  QMimeData const *md = event->mimeData();
  if ( md->hasFormat( "FabricUI/PortLabel" ) )
  {
    QByteArray ba = md->data( "FabricUI/PortLabel" );
    QDataStream ds( ba );
    char *portLabelCharPtr;
    ds >> portLabelCharPtr;
    // TODO check portLabel match

    QList<QGraphicsItem *> ports = m_itemGroup->childItems();

    // Start at 1: skip proxy port
    qreal eventY = event->pos().y();
    int oldDragTargetIndex = m_dragTargetIndex;
    m_dragTargetIndex = -1;
    qreal bestDist;
    for ( int i = 1; i < ports.size(); ++i )
    {
      QGraphicsItem *port = ports[i];
      qreal portY = m_itemGroup->mapToParent( port->pos() ).y();
      if ( m_dragTargetIndex == -1
        || fabs( portY - eventY ) < bestDist )
      {
        bestDist = fabs( portY - eventY );
        m_dragTargetIndex = i;
      }
    }

    QGraphicsItem *lastPort = ports.back();
    qreal lastPortBottomY =
        m_itemGroup->mapToParent( lastPort->pos() ).y()
      + lastPort->boundingRect().height() + 8;
    if ( m_dragTargetIndex == -1
      || fabs( lastPortBottomY - eventY ) < bestDist )
    {
      bestDist = fabs( lastPortBottomY - eventY );
      m_dragTargetIndex = ports.size();
    }

    // qDebug() << "m_dragTargetIndex: " << m_dragTargetIndex;
    if ( m_dragTargetIndex != oldDragTargetIndex )
      update();

    event->acceptProposedAction();
    event->accept();
    return;
  }

  QGraphicsWidget::dragMoveEvent( event );
}

void SidePanel::dragLeaveEvent( QGraphicsSceneDragDropEvent *event )
{
  if ( m_dragTargetIndex != -1 )
  {
    m_dragTargetIndex = -1;
    update();
  }

  QGraphicsWidget::dragLeaveEvent( event );
}

void SidePanel::dropEvent( QGraphicsSceneDragDropEvent *event )
{
  if ( m_dragTargetIndex != -1 )
  {
    m_dragTargetIndex = -1;
    update();
  }

  QGraphicsWidget::dropEvent( event );
}
