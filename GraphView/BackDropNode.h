// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_BackDropNode__
#define __UI_GraphView_BackDropNode__

#include <FabricUI/GraphView/GraphicItemTypes.h>
#include <FabricUI/GraphView/Node.h>
#include <FTL/JSONValue.h>

namespace FabricUI
{

  namespace GraphView
  {

    class BackDropNode : public Node
    {
    public:

      BackDropNode(
        Graph * parent,
        FTL::CStrRef name,
        FTL::CStrRef title = FTL::CStrRef(),
        QColor color = QColor( 0x62, 0x67, 0x6B, 0xA0 ),
        QColor titleColor = QColor( 0x62, 0x67, 0x6B, 0xB0 )
        );
      virtual ~BackDropNode();

      void setSize( QSizeF size );

      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

      void appendOverlappingNodes( std::vector<Node*> &nodes ) const;
      
    private:

      int getCorner(QPointF pos);
      bool commentExpanded() const;

      float m_resizeDistance;
      bool m_hasCustomPointer;
    };


  };

};

#endif // __UI_GraphView_BackDropNode__
