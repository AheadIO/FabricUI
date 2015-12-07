// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphicItemTypes__
#define __UI_GraphView_GraphicItemTypes__

#include <QtGui/QGraphicsItem>

namespace FabricUI
{

  namespace GraphView
  {

    enum GraphicsItemTypes
    {
      QGraphicsItemType_Node = QGraphicsItem::UserType + 5,
      QGraphicsItemType_Pin,
      QGraphicsItemType_Port,
      QGraphicsItemType_ProxyPort,
      QGraphicsItemType_Connection,
      QGraphicsItemType_PinCircle,
      QGraphicsItemType_NodeRectangle,
      QGraphicsItemType_ProxyPin,
      QGraphicsItemType_NodeHeaderButton
    };

  };

};

#endif // __UI_GraphView_GraphicItemTypes__
