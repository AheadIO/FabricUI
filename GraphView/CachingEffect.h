// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_CachingEffect__
#define __UI_GraphView_CachingEffect__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsEffect>

namespace FabricUI
{

  namespace GraphView
  {
    class CachingEffect : public QGraphicsEffect
    {
      Q_OBJECT

    public:

      CachingEffect(QGraphicsWidget * parent);
      virtual ~CachingEffect() {}

      virtual void draw(QPainter * painter);
    };

  };

};

#endif // __UI_GraphView_CachingEffect__
