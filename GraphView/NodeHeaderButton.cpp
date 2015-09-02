// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NodeHeaderButton.h"
#include "NodeHeader.h"
#include "Node.h"
#include "Graph.h"

#include <QtGui/QPainter>
#include <QtGui/QGraphicsSceneMouseEvent>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

#include <stdio.h>

using namespace FabricUI::GraphView;

std::map<QString, QPixmap> NodeHeaderButton::s_pixmaps;

NodeHeaderButton::NodeHeaderButton(NodeHeader * parent, QString name, QStringList icons, int state)
: QGraphicsWidget(parent)
{
  m_nodeHeader = parent;
  m_name = name;
  m_icons = icons;
  m_state = 0;

  for(unsigned int i=0;i<icons.count();i++)
  {
    QString icon = icons[i];
    std::map<QString, QPixmap>::iterator it = s_pixmaps.find(icon);
    QPixmap pixmap;
    if(it != s_pixmaps.end())
      pixmap = it->second;
    else
    {
      QString filePath = m_nodeHeader->node()->graph()->config().nodeHeaderButtonIconDir + icon;

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

      pixmap = QPixmap(filePath);
      s_pixmaps.insert(std::pair<QString, QPixmap>(icon, pixmap));
    
      if(pixmap.width() == 0)
      {
        printf("NodeHeaderButton: Pixmap not found: '%s'\n", filePath.toUtf8().constData());
        return;
      }
    }

    m_pixmaps.append(pixmap);
  }

  setMinimumWidth(m_pixmaps[m_state].width());
  setMaximumWidth(m_pixmaps[m_state].width());
  setMinimumHeight(m_pixmaps[m_state].height());
  setMaximumHeight(m_pixmaps[m_state].height());

  setState(state);
}

void NodeHeaderButton::setState(int value)
{
  if(m_state == value)
    return;
  if(m_state >= m_pixmaps.count())
    return;
  m_state = value;
  update();
}

void NodeHeaderButton::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(event->button() == Qt::LeftButton)
  {
    m_lastMousePos = event->scenePos();
    event->accept();

    m_nodeHeader->node()->onMousePress(
      event->button(),
      event->modifiers(),
      event->scenePos(),
      event->lastScenePos()
      );
    return;
  }
  QGraphicsWidget::mousePressEvent(event);
}

void NodeHeaderButton::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  event->accept();

  // force the event to fall back to the node
  // this is done so that dragging etc works correctly.
  m_nodeHeader->node()->onMouseMove(
    event->button(),
    event->modifiers(),
    event->scenePos(),
    event->lastScenePos()
    );
}

void NodeHeaderButton::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_lastMousePos == event->scenePos())
  {
    emit triggered(this);
  }

  // force the event to fall back to the node
  // this is done so that dragging etc works correctly.
  m_nodeHeader->node()->onMouseRelease(
    event->button(),
    event->modifiers(),
    event->scenePos(),
    event->lastScenePos()
    );
}

void NodeHeaderButton::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
#ifdef FABRICUI_TIMERS
  Util::TimerPtr timer = Util::Timer::getTimer("FabricUI::NodeHeaderButton");
  timer->resume();
#endif

  if(m_pixmaps.count() > m_state)
  {
    QPixmap pixmap = m_pixmaps[m_state];
    painter->drawPixmap(QPointF(0, 0), pixmap);
  }

  QGraphicsWidget::paint(painter, option, widget);
}
