//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/GraphView/HighlightEffect.h>

#include <QtGui/QPainter>

namespace FabricUI {

HighlightEffect::HighlightEffect( QColor const &color, QObject *parent )
  : QGraphicsEffect( parent )
{
  QColor brushColor( color );
  brushColor.setAlpha( 128 );
  m_brush = QBrush( brushColor );
}

HighlightEffect::~HighlightEffect()
{
}

QRectF HighlightEffect::boundingRectFor( QRectF const &rect ) const
{
  return rect.adjusted( -2, -8, +2, +8 );
}

void HighlightEffect::draw( QPainter *painter )
{
  QPoint offset;
  QPixmap pixmap = sourcePixmap( Qt::DeviceCoordinates, &offset );
  QRect rect = pixmap.rect();
  rect.moveTopLeft( offset );

  painter->save();
  painter->setWorldTransform( QTransform() );
  painter->setPen( Qt::NoPen );
  painter->setBrush( m_brush );
  painter->drawRoundedRect( rect, 4, 4 );
  painter->drawPixmap( offset, pixmap );
  painter->restore();
}

} // namespace FabricUI;
