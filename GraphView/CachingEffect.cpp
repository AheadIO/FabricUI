// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "CachingEffect.h"

#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>

using namespace FabricUI::GraphView;

CachingEffect::CachingEffect(QGraphicsWidget * parent)
: QGraphicsEffect(parent)
{
  QPixmapCache::setCacheLimit(10240 * 10);
}

void CachingEffect::draw(QPainter * painter)
{
  QPoint point;
  QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &point, NoPad);
  painter->drawPixmap(point, pixmap);
}
