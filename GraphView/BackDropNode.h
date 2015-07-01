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

      virtual void setTopLeftGraphPos(QPointF pos, bool quiet = false);
      void setSize( QSizeF size, bool quiet );

      virtual bool isBackDropNode() const { return true; }
      virtual bool supportsToolBar() const { return false; }

      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    private:

      std::vector<Node*> getOverlappingNodes() const;
      int getCorner(QPointF pos);
      void setSizeFromMouse(float width, float height);
      bool commentExpanded() const;

      float m_resizeDistance;
      bool m_shiftPressed;
      bool m_hasCustomPointer;
      std::vector<Node*> m_overlappingNodes;
    };


  };

};

#endif // __UI_GraphView_BackDropNode__
