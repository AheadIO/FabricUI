// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_BackDropNode__
#define __UI_GraphView_BackDropNode__

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
        FTL::CStrRef title,
        QColor color = QColor(),
        QColor titleColor = QColor()
        );
      virtual ~BackDropNode();

      virtual void setColor(QColor col);
      virtual int type() const { return QGraphicsItemType_BackDropNode; }

      static QString getDefaultJSON(char const * name, char const * title, QPointF pos);
      QString getJSON() const;
      QString getJSON(QString t) const;
      QString getJSON(QPointF p) const;
      QString getJSON(QSizeF s) const;
      QString getJSON(QColor c) const;
      void updateFromJSON(const QString & json);
      void updateFromJSON(FTL::JSONObject const *jsonObject);

      virtual void setTopLeftGraphPos(QPointF pos, bool quiet = false);

      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    private:

      static QString getJSON(FTL::CStrRef name, FTL::CStrRef title, QPointF p, QSizeF s, QColor c);

      std::vector<Node*> getOverlappingNodes() const;
      int getCorner(QPointF pos);
      void setSizeFromMouse(float width, float height);

      float m_resizeDistance;
      bool m_hasCustomPointer;
      std::vector<Node*> m_overlappingNodes;
    };


  };

};

#endif // __UI_GraphView_BackDropNode__
