// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_MainPanelBackground__
#define __UI_GraphView_MainPanelBackground__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QPen>
#include <QtGui/QColor>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class MainPanel;
    class CachingEffect;

    class MainPanelBackground : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      MainPanelBackground(MainPanel * parent);
      virtual ~MainPanelBackground() {}

      virtual const QTransform & offset() const;
      virtual void setOffset(QTransform transform);

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    private:

      MainPanel * m_panel;
      QTransform m_offset;

    };

  };

};

#endif // __UI_GraphView_MainPanelBackground__
