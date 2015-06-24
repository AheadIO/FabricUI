// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphicItemTypes__
#define __UI_GraphView_GraphicItemTypes__

namespace FabricUI
{

  namespace GraphView
  {

    const int QGraphicsItemType_Node = QGraphicsItem::UserType + 5;
    const int QGraphicsItemType_Pin = QGraphicsItem::UserType + 6;
    const int QGraphicsItemType_Port = QGraphicsItem::UserType + 7;
    const int QGraphicsItemType_ProxyPort = QGraphicsItem::UserType + 8;
    const int QGraphicsItemType_Connection = QGraphicsItem::UserType + 9;
    const int QGraphicsItemType_PinCircle = QGraphicsItem::UserType + 10;
    const int QGraphicsItemType_NodeRectangle = QGraphicsItem::UserType + 11;
    const int QGraphicsItemType_BackDropNode = QGraphicsItem::UserType + 12;

  };

};

#endif // __UI_GraphView_GraphicItemTypes__
