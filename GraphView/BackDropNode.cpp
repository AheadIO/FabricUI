// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QApplication>
#include <QtGui/QGraphicsSceneHoverEvent>

#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/Graph.h>

using namespace FabricUI::GraphView;

BackDropNode::BackDropNode(
  Graph * parent,
  FTL::CStrRef name,
  FTL::CStrRef title,
  QColor color,
  QColor titleColor
  )
  : Node( parent, name, title, color, titleColor )
{
  m_mainWidget->setMinimumWidth(graph()->config().nodeMinWidth * 2.0f);
  m_mainWidget->setMinimumHeight(graph()->config().nodeMinHeight * 2.0f);
  m_mainWidget->setMaximumWidth(graph()->config().nodeMinWidth * 2.0f);
  m_mainWidget->setMaximumHeight(graph()->config().nodeMinHeight * 2.0f);

  m_resizeDistance = 16.0;
  m_hasCustomPointer = false;
  setAcceptHoverEvents(true);
  setZValue(-100);
}

BackDropNode::~BackDropNode()
{
}

void BackDropNode::setColor(QColor col)
{
  col.setAlphaF(graph()->config().backDropNodeAlpha);
  Node::setColor(col);
  Node::setTitleColor(col.darker(130));
}

QString BackDropNode::getDefaultJSON(char const * name, char const * title, QPointF pos)
{
  QString xStr = QString::number(pos.x());
  QString yStr = QString::number(pos.y());
  QString keyStr = "uiBackDrop_"+QString(name);
  QString jsonStr = "{\"name\": \"" + QString(name) + "\", " + 
    "\"title\": \"" + QString(title) + "\", " +
    "\"pos\": {\"x\": " + xStr +", \"y\": " + yStr + "}" + 
    QString("}");
  return jsonStr;
}

QString BackDropNode::getJSON() const
{
  QPointF pos = topLeftGraphPos();
  QSizeF s = size();
  QColor c = color();
  return getJSON(name(), title(), topLeftGraphPos(), size(), color());
}

QString BackDropNode::getJSON(QString t) const
{
  return getJSON(name(), t.toUtf8().constData(), topLeftGraphPos(), size(), color());
}

QString BackDropNode::getJSON(QPointF p) const
{
  return getJSON(name(), title(), p, size(), color());
}

QString BackDropNode::getJSON(QSizeF s) const
{
  return getJSON(name(), title(), topLeftGraphPos(), s, color());
}

QString BackDropNode::getJSON(QColor c) const
{
  return getJSON(name(), title(), topLeftGraphPos(), size(), c);
}

QString BackDropNode::getJSON(FTL::CStrRef name, FTL::CStrRef title, QPointF p, QSizeF s, QColor c)
{
  QString keyStr = "uiBackDrop_" + QString(name.c_str());
  QString jsonStr = "{\"name\": \"" + QString(name.c_str()) + "\", " +
    "\"title\": \"" + QString(title.c_str()) + "\", " +
    "\"pos\": {\"x\": " + QString::number(p.x()) +", \"y\": " + QString::number(p.y()) + "}," + 
    "\"size\": {\"width\": " + QString::number(s.width()) +", \"height\": " + QString::number(s.height()) + "}," + 
    "\"color\": {\"r\": " + QString::number(c.red()) +", \"g\": " + QString::number(c.green()) + ", " + 
      "\"b\": " + QString::number(c.blue()) +", \"a\": " + QString::number(c.alpha()) + "}" + 
    QString("}");
  return jsonStr;
}

void BackDropNode::updateFromJSON(const QString & json)
{
  FTL::CStrRef jsonStr( json.toUtf8().constData() );
  FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr.c_str() );
  FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
    FTL::JSONValue::Decode( jsonStrWithLoc )
    );
  if ( jsonValue )
  {
    FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
    updateFromJSON(jsonObject);
  }
}

void BackDropNode::updateFromJSON(FTL::JSONObject const *jsonObject)
{
  FTL::CStrRef title = jsonObject->getString( FTL_STR("title") );
  setTitle(title);

  FTL::JSONObject const *posObject = jsonObject->maybeGetObject( FTL_STR("pos") );
  if(posObject)
  {
    QPointF pos(
      (float)posObject->getFloat64( FTL_STR("x") ),
      (float)posObject->getFloat64( FTL_STR("y") )
      );
    setTopLeftGraphPos(pos);
  }

  FTL::JSONObject const *sizeObject = jsonObject->maybeGetObject( FTL_STR("size") );
  if(sizeObject)
  {
    QSizeF size(
      (float)sizeObject->getFloat64( FTL_STR("width") ),
      (float)sizeObject->getFloat64( FTL_STR("height") )
      );
    m_mainWidget->setMinimumWidth(size.width());
    m_mainWidget->setMinimumHeight(size.height());
    m_mainWidget->setMaximumWidth(size.width());
    m_mainWidget->setMaximumHeight(size.height());
  }

  FTL::JSONObject const *colorObject = jsonObject->maybeGetObject( FTL_STR("color") );
  if(colorObject)
  {
    QColor color(
      colorObject->getSInt32( FTL_STR("r") ),
      colorObject->getSInt32( FTL_STR("g") ),
      colorObject->getSInt32( FTL_STR("b") ),
      colorObject->getSInt32( FTL_STR("a") )
      );
    setColor(color);
  }

}

void BackDropNode::setTopLeftGraphPos(QPointF pos, bool quiet)
{
  QPointF prev = topLeftGraphPos();
  Node::setTopLeftGraphPos(pos, quiet);
  QPointF delta = topLeftGraphPos() - prev;

  for(size_t i=0;i<m_overlappingNodes.size();i++)
    graph()->controller()->moveNode(m_overlappingNodes[i], m_overlappingNodes[i]->topLeftGraphPos() + delta, true);
}

void BackDropNode::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  if(m_hasCustomPointer)
  {
    QApplication::restoreOverrideCursor();
    m_hasCustomPointer = false;
  }
  Node::hoverLeaveEvent(event);
}

void BackDropNode::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  if(m_dragging == 0)
  {
    int corner = getCorner(event->pos());

    if(!m_hasCustomPointer && (corner == 0 || corner == 3))
    {
      QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
      m_hasCustomPointer = true;
    }
    else if(!m_hasCustomPointer && (corner == 1 || corner == 2))
    {
      QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
      m_hasCustomPointer = true;
    }
    else if(m_hasCustomPointer)
    {
      QApplication::restoreOverrideCursor();
      m_hasCustomPointer = false;
    }
  }

  Node::hoverMoveEvent(event);
}

void BackDropNode::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  int corner = getCorner(event->pos());
  if(corner != -1)
  {
    m_dragging = 3 + corner;
    m_lastDragPoint = mapToScene(event->pos());
    event->accept();
    return;
  }
  Node::mousePressEvent(event);

  if(m_dragging == 1)
  {
    m_overlappingNodes = BackDropNode::getOverlappingNodes();
  }
}

void BackDropNode::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_dragging == 3) // topleft
  {
    QPointF delta = mapToScene(event->pos()) - m_lastDragPoint;
    graph()->controller()->moveNode(this, topLeftGraphPos() + delta, true);
    QSizeF s = m_mainWidget->size();
    setSizeFromMouse(s.width() - delta.x(), s.height() - delta.y());
    m_lastDragPoint += delta;
    event->accept();
    return;
  }
  else if(m_dragging == 4) // topright
  {
    QPointF delta = mapToScene(event->pos()) - m_lastDragPoint;
    graph()->controller()->moveNode(this, topLeftGraphPos() + QPointF(0.0f, delta.y()), true);
    QSizeF s = m_mainWidget->size();
    setSizeFromMouse(s.width() + delta.x(), s.height() - delta.y());
    m_lastDragPoint += delta;
    event->accept();
    return;
  }
  else if(m_dragging == 5) // bottomleft
  {
    QPointF delta = mapToScene(event->pos()) - m_lastDragPoint;
    graph()->controller()->moveNode(this, topLeftGraphPos() + QPointF(delta.x(), 0.0f), true);
    QSizeF s = m_mainWidget->size();
    setSizeFromMouse(s.width() - delta.x(), s.height() + delta.y());
    m_lastDragPoint += delta;
    event->accept();
    return;
  }
  else if(m_dragging == 6) // bottomright
  {
    QPointF delta = mapToScene(event->pos()) - m_lastDragPoint;
    QSizeF s = m_mainWidget->size();
    setSizeFromMouse(s.width() + delta.x(), s.height() + delta.y());
    m_lastDragPoint += delta;
    event->accept();
    return;
  }

  Node::mouseMoveEvent(event);
}

void BackDropNode::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_dragging >= 3)
  {
    m_dragging = 0;
    event->accept();
    return;
  }
  Node::mouseReleaseEvent(event);
  m_overlappingNodes.clear();
}

std::vector<Node*> BackDropNode::getOverlappingNodes() const
{
  std::vector<Node *> all = graph()->nodes();
  std::vector<Node *> result;

  QPointF topLeft = mapToScene(boundingRect().topLeft());
  QPointF bottomRight = mapToScene(boundingRect().bottomRight());
  QRectF rect(topLeft, bottomRight);

  for(size_t i=0;i<all.size();i++)
  {
    if(all[i]->type() == QGraphicsItemType_BackDropNode)
      continue;

    QPointF topLeft2 = all[i]->mapToScene(all[i]->boundingRect().topLeft());
    QPointF bottomRight2 = all[i]->mapToScene(all[i]->boundingRect().bottomRight());
    QRectF rect2(topLeft2, bottomRight2);

    if(rect.contains(rect2))
      result.push_back(all[i]);
  }

  return result;
}

int BackDropNode::getCorner(QPointF pos)
{
  QPointF topLeft = mapFromItem(m_mainWidget, m_mainWidget->boundingRect().topLeft());
  QPointF bottomRight = mapFromItem(m_mainWidget, m_mainWidget->boundingRect().bottomRight());
  QRectF rect(topLeft, bottomRight);

  if((rect.topLeft() - pos).manhattanLength() < m_resizeDistance)
    return 0;
  else if((rect.topRight() - pos).manhattanLength() < m_resizeDistance)
    return 1;
  else if((rect.bottomLeft() - pos).manhattanLength() < m_resizeDistance)
    return 2;
  else if((rect.bottomRight() - pos).manhattanLength() < m_resizeDistance)
    return 3;

  return -1;
}

void BackDropNode::setSizeFromMouse(float width, float height)
{
  if(width < graph()->config().nodeMinWidth * 2.0f)
    width = graph()->config().nodeMinWidth * 2.0f;
  if(height < graph()->config().nodeMinHeight * 2.0f)
    height = graph()->config().nodeMinHeight * 2.0f;

  graph()->controller()->setBackDropNodeSize(this, QSizeF(width, height));
}
