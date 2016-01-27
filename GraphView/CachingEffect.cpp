// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/CachingEffect.h>

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
  QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &point, NoPad);
  painter->setWorldTransform(QTransform());
  painter->drawPixmap(point, pixmap);
}
