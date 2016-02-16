//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _FabricUI_DFG_HighlightEffect_h
#define _FabricUI_DFG_HighlightEffect_h

#include <QtGui/QBrush.h>
#include <QtGui/QGraphicsEffect.h>

namespace FabricUI {

class HighlightEffect : public QGraphicsEffect
{
public:

  HighlightEffect( QColor const &color, QObject *parent = 0 );
  ~HighlightEffect();

  virtual QRectF boundingRectFor( QRectF const &rect ) const /*override*/;
  virtual void draw( QPainter *painter ) /*override*/;

private:

  QBrush m_brush;
};

} // namespace FabricUI;

#endif // _FabricUI_DFG_HighlightEffect_h
