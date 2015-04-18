// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Node__
#define __UI_GraphView_Node__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include "NodeRectangle.h"
#include "NodeHeader.h"
#include "Pin.h"
#include "GraphicItemTypes.h"
#include "CachingEffect.h"

namespace FabricUI
{

  namespace GraphView
  {

    // forward declarations
    class Graph;

    class Node : public QGraphicsWidget
    {
      Q_OBJECT

      friend class NodeRectangle;

    public:

      enum CollapseState
      {
        CollapseState_Expanded,
        CollapseState_OnlyConnections,
        CollapseState_Collapsed,
        CollapseState_NumStates
      };

      Node(Graph * parent, QString path, QString label = "", QColor color = QColor(), QColor labelColor = QColor());
      virtual ~Node();

      virtual int type() const { return QGraphicsItemType_Node; }

      Graph * graph();
      const Graph * graph() const;
      NodeHeader * header();
      const NodeHeader * header() const;

      QString path() const;
      QString name() const;
      QString title() const;
      void setTitle(QString t);
      QString preset() const;
      QColor color() const;
      void setColor(QColor col);
      void setColorAsGradient(QColor a, QColor b);
      QColor labelColor() const;
      void setLabelColor(QColor col);
      QPen defaultPen() const;
      QPen selectedPen() const;

      virtual bool selected() const;

      virtual CollapseState collapsedState() const;
      virtual void setCollapsedState(CollapseState state);
      virtual void toggleCollapsedState();

      virtual QString error() const;
      virtual bool hasError() const;
      virtual void setError(QString text);
      virtual void clearError();

      virtual QPointF graphPos() const;
      virtual QPointF topLeftGraphPos() const;
      virtual QPointF topLeftToCentralPos(QPointF pos);
      virtual QPointF centralPosToTopLeftPos(QPointF pos);

      virtual unsigned int pinCount() const;
      virtual Pin * pin(unsigned int index);
      virtual Pin * pin(QString name);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

      // accessed by controller
      virtual void setPreset(QString p);
      virtual void setSelected(bool state, bool quiet = false);
      virtual void setGraphPos(QPointF pos, bool quiet = false);
      virtual void setTopLeftGraphPos(QPointF pos, bool quiet = false);
      virtual Pin * addPin(Pin * pin, bool quiet = false);
      virtual bool removePin(Pin * pin, bool quiet = false);

      QGraphicsWidget * mainWidget();
      QGraphicsWidget * pinsWidget();

    public slots:

      void onConnectionsChanged();
      
    signals:

      void selectionChanged(FabricUI::GraphView::Node *, bool);
      void collapsedStateChanged(FabricUI::GraphView::Node *, FabricUI::GraphView::Node::CollapseState);
      void positionChanged(FabricUI::GraphView::Node *, QPointF);
      void pinAdded(FabricUI::GraphView::Node *, Pin *);
      void pinRemoved(FabricUI::GraphView::Node *, Pin *);
      void doubleClicked(FabricUI::GraphView::Node *);

    protected:

      void updatePinLayout();

      Graph * m_graph;
      QString m_path;
      QString m_preset;
      QString m_labelCaption;

      QColor m_colorA;
      QColor m_colorB;
      QColor m_labelColor;
      QPen m_defaultPen;
      QPen m_selectedPen;
      QPen m_errorPen;
      float m_cornerRadius;
      QString m_errorText;
      CollapseState m_collapsedState;

      NodeHeader * m_header;
      NodeRectangle * m_mainWidget;
      QGraphicsWidget * m_pinsWidget;
      QGraphicsLinearLayout * m_pinsLayout;
      bool m_selected;
      int m_dragging;
      QPointF m_lastDragPoint;

      std::vector<Pin*> m_pins;
      CachingEffect * m_cache;
    };


  };

};

#endif // __UI_GraphView_Node__
