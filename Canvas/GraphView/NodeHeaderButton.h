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

      NodeHeaderButton(NodeHeader * parent, QString name, QStringList icons, int state = 0);
      virtual ~NodeHeaderButton() {}

      virtual int type() const { return QGraphicsItemType_NodeHeaderButton; }
      NodeHeader * header() { return m_nodeHeader; }
      virtual QString name() const { return m_name; }
      virtual QString icon() const { return m_icons[m_state]; }
      virtual int state() const { return m_state; }
      virtual void setState(int value);
      virtual bool highlighted() const { return m_highlighted; }
      virtual void setHighlighted(bool value);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    signals:

      void triggered(FabricUI::GraphView::NodeHeaderButton * button);

    private:

      NodeHeader * m_nodeHeader;
      QString m_name;
      QStringList m_icons;
      int m_state;
      bool m_highlighted;
      QList<QPixmap> m_pixmaps;
      QPointF m_lastMousePos;

      static std::map<QString, QPixmap> s_pixmaps;

      static const int s_pixmapSize = 12;

    };


  };

};

#endif // __UI_GraphView_NodeHeaderButton__
