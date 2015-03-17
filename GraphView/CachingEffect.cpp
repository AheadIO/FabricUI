// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "CachingEffect.h"

#include <QtGui/QPainter>
#include <QtGui/QPixmap>

using namespace FabricUI::GraphView;

CachingEffect::CachingEffect(QGraphicsWidget * parent)
: QGraphicsEffect(parent)
{
}

void CachingEffect::draw(QPainter * painter)
{
  QPoint point;
  QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &point);
  painter->drawPixmap(point, pixmap);
}
