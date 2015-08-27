// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeHeaderButton__
#define __UI_GraphView_NodeHeaderButton__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include "GraphicItemTypes.h"

namespace FabricUI
{

  namespace GraphView
  {

    // forward declarations
    class NodeHeader;

    class NodeHeaderButton : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      NodeHeaderButton(NodeHeader * parent, QString name, QString icon, int rotation = 0);
      virtual ~NodeHeaderButton() {}

      virtual int type() const { return QGraphicsItemType_NodeHeaderButton; }
      NodeHeader * header() { return m_nodeHeader; }
      virtual QString name() const { return m_name; }
      virtual QString icon() const { return m_icon; }
      virtual int rotation() const { return m_rotation; }
      virtual void setRotation(int rot);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    signals:

      void triggered(FabricUI::GraphView::NodeHeaderButton * button);

    private:

      NodeHeader * m_nodeHeader;
      QString m_name;
      QString m_icon;
      int m_rotation;
      QPixmap m_pixmap;

      static std::map<QString, QPixmap> s_pixmaps;

    };


  };

};

#endif // __UI_GraphView_NodeHeaderButton__
