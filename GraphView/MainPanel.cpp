// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsSceneWheelEvent>
#include <QtGui/QPainter>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsView>

#include "MainPanel.h"
#include "Pin.h"
#include "Node.h"
#include "Graph.h"
#include "GraphConfig.h"
#include "CachingEffect.h"

using namespace FabricUI::GraphView;

MainPanel::MainPanel(Graph * parent)
: QGraphicsWidget(parent)
{
  m_graph = parent;
  m_mouseWheelZoomState = 1.0;
  m_mouseWheelZoomRate = m_graph->config().mouseWheelZoomRate;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  // m_backGround = new MainPanelBackground(this);
  m_itemGroup = new QGraphicsWidget(this);
  // m_itemGroup = new QGraphicsWidget(parent);

  m_manipulationMode = ManipulationMode_None;
  m_draggingSelRect = false;
  m_selectionRect = NULL;
  m_spaceBarDown = false;
}

Graph * MainPanel::graph()
{
  return m_graph;
}

const Graph * MainPanel::graph() const
{
  return m_graph;
}

QGraphicsWidget * MainPanel::itemGroup()
{
  return m_itemGroup;
}

const QGraphicsWidget * MainPanel::itemGroup() const
{
  return m_itemGroup;
}

float MainPanel::mouseWheelZoomRate() const
{
  return m_mouseWheelZoomRate;
}

void MainPanel::setMouseWheelZoomRate(float rate)
{
  m_mouseWheelZoomRate = rate;
}

float MainPanel::canvasZoom() const
{
  return m_mouseWheelZoomState;
}

void MainPanel::setCanvasZoom(float state, bool quiet)
{
  if(state > 1.0)
    state = 1.0;
  if(state < 0.05)
    state = 0.05;
  if(m_mouseWheelZoomState == state)
    return;

  QPointF cursorPos = m_mouseWheelZoomState * m_lastPanPoint;

  m_mouseWheelZoomState = state;

  QGraphicsView * graphicsView = graph()->scene()->views()[0];
  if(m_mouseWheelZoomState == 1.0)
    graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, false);
  else
    graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);

  m_itemGroup->setScale(m_mouseWheelZoomState);

  QPointF newCursorPos = m_mouseWheelZoomState * m_lastPanPoint;
  setCanvasPan(canvasPan() + cursorPos - newCursorPos);

  update();

  if(!quiet)
    emit canvasZoomChanged(m_mouseWheelZoomState);
}

QPointF MainPanel::canvasPan() const
{
  return m_itemGroup->pos();
}

void MainPanel::setCanvasPan(QPointF pos, bool quiet)
{
  m_itemGroup->setPos(pos);

  if(!quiet)
    emit canvasPanChanged(pos);
}

MainPanel::ManipulationMode MainPanel::manipulationMode() const
{
  return m_manipulationMode;
}

QRectF MainPanel::boundingRect() const
{
  return windowFrameRect();
}

void MainPanel::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(event->button() == Qt::LeftButton && !m_spaceBarDown)
  {
    QPointF mouseDownPos = mapToItem(m_itemGroup, event->pos());
    m_selectionRect = new SelectionRect(this, mouseDownPos);
    m_draggingSelRect = false;

    if(!event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier))
      m_graph->controller()->clearSelection();
    m_ongoingSelection.clear();

    m_manipulationMode = ManipulationMode_Select;
  }
  else if((event->button() == DFG_QT_MIDDLE_MOUSE && event->modifiers().testFlag(Qt::AltModifier)) || (event->button() == Qt::LeftButton && m_spaceBarDown))
  {
    setCursor(Qt::OpenHandCursor);
    m_manipulationMode = ManipulationMode_Pan;
    m_lastPanPoint = event->pos();
  }
  else if(event->button() == Qt::RightButton && event->modifiers().testFlag(Qt::AltModifier))
  {
    setCursor(Qt::OpenHandCursor);
    m_manipulationMode = ManipulationMode_Zoom;
    m_lastPanPoint = event->pos();
    m_mouseAltZoomState = m_mouseWheelZoomState;
  }
  else if(event->button() == Qt::RightButton)
  {
    QMenu * menu = graph()->getGraphContextMenu();
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

void MainPanel::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_manipulationMode == ManipulationMode_Select)
  {
    QPointF dragPoint = mapToItem(m_itemGroup, event->pos());
    m_selectionRect->setDragPoint(dragPoint);

    if(!event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier))
      m_graph->controller()->clearSelection();
    else
    {
      for(size_t i=0;i<m_ongoingSelection.size();i++)
        m_graph->controller()->selectNode(m_ongoingSelection[i], false);
    }

    m_ongoingSelection.clear();
    std::vector<Node*> nodes = m_graph->nodes();
    for(size_t i=0;i<nodes.size();i++)
    {
      if(m_selectionRect->collidesWithItem(nodes[i]))
      {
        if(!nodes[i]->selected())
        {
          m_graph->controller()->selectNode(nodes[i], true);
          m_ongoingSelection.push_back(nodes[i]);
        }
      }
    }
    m_draggingSelRect = true;
  }
  else if(m_manipulationMode == ManipulationMode_Pan)
  {
    QTransform xfo = m_itemGroup->transform().inverted();
    QPointF delta = xfo.map(event->pos()) - xfo.map(m_lastPanPoint);
    m_lastPanPoint = event->pos();
    if(m_graph->controller()->panCanvas(delta + canvasPan()))
    {
      // if(!m_graph->config().mainPanelBackGroundPanFixed)
      // m_backGround->setOffset(m_itemGroup->transform());
      update();
    }
  }
  else if(m_manipulationMode == ManipulationMode_Zoom)
  {
    QPointF delta = (event->pos() - m_lastPanPoint) * m_mouseAltZoomState;

    float zoomFactor = 1.0f - 3.5 * (delta.y() - delta.x()) * m_mouseWheelZoomRate;
    float newZoomState = m_mouseAltZoomState * zoomFactor;
    m_graph->controller()->zoomCanvas(newZoomState);
  }
  else
    QGraphicsWidget::mouseMoveEvent(event);
}

void MainPanel::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_manipulationMode == ManipulationMode_Select)
  {
    prepareGeometryChange();
  
    scene()->removeItem(m_selectionRect);
    delete(m_selectionRect);
    m_selectionRect = NULL;
    // if(!m_draggingSelRect)
    //   m_graph.clearSelection()
    m_manipulationMode = ManipulationMode_None;
  }
  else if(m_manipulationMode == ManipulationMode_Pan || m_manipulationMode == ManipulationMode_Zoom)
  {
    setCursor(Qt::ArrowCursor);
    m_manipulationMode = ManipulationMode_None;
  }
  else
    QGraphicsWidget::mouseMoveEvent(event);
}

void MainPanel::wheelEvent(QGraphicsSceneWheelEvent * event)
{
  if(m_manipulationMode == ManipulationMode_None)
  {
    m_lastPanPoint = event->pos();
    float zoomFactor = 1.0f + float(event->delta()) * m_mouseWheelZoomRate;
    float newZoomState = m_mouseWheelZoomState * zoomFactor;
    m_graph->controller()->zoomCanvas(newZoomState);
  }
}

void MainPanel::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  // QRectF rect = windowFrameRect();
  // m_backGround->resize(rect.width(), rect.height());
  QGraphicsWidget::paint(painter, option, widget);
}
