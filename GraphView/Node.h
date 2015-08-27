// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Node__
#define __UI_GraphView_Node__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include <FTL/CStrRef.h>

#include <FabricUI/GraphView/NodeRectangle.h>
#include <FabricUI/GraphView/NodeHeader.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/GraphicItemTypes.h>
#include <FabricUI/GraphView/CachingEffect.h>

namespace FabricUI
{

  namespace GraphView
  {

    // forward declarations
    class Graph;
    class NodeBubble;

    class Node : public QGraphicsWidget
    {
      Q_OBJECT

      friend class NodeRectangle;
      friend class NodeBubble;

    public:

      enum CollapseState
      {
        CollapseState_Expanded,
        CollapseState_OnlyConnections,
        CollapseState_Collapsed,
        CollapseState_NumStates
      };

      Node(
        Graph * parent,
        FTL::CStrRef name,
        FTL::CStrRef title,
        QColor color = QColor(),
        QColor titleColor = QColor(),
        bool isBackDropNode = false
        );
      virtual ~Node();

      virtual int type() const { return QGraphicsItemType_Node; }
      bool isBackDropNode() const { return m_isBackDropNode; }

      Graph * graph();
      const Graph * graph() const;
      NodeHeader * header();
      const NodeHeader * header() const;
      NodeBubble * bubble();
      const NodeBubble * bubble() const;

      FTL::CStrRef name() const
        { return m_name; }
      
      FTL::CStrRef title() const
        { return m_title; }
      void setTitle( FTL::CStrRef title );
      QColor color() const;
      virtual void setColor(QColor col);
      virtual void setColorAsGradient(QColor a, QColor b);
      QColor titleColor() const;
      virtual void setTitleColor(QColor col);
      QColor fontColor() const;
      virtual void setFontColor(QColor col);
      QPen defaultPen() const;
      QPen selectedPen() const;
      QString comment() const;

      virtual QRectF boundingRect() const;
      
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
      virtual QPointF topLeftToCentralPos(QPointF pos) const;
      virtual QPointF centralPosToTopLeftPos(QPointF pos) const;

      virtual unsigned int pinCount() const;
      virtual Pin * pin(unsigned int index);
      virtual Pin * pin(FTL::StrRef name);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

      // accessed by controller
      virtual void setSelected(bool state, bool quiet = false);
      virtual void setGraphPos(QPointF pos, bool quiet = false);
      void setTopLeftGraphPos(QPointF pos, bool quiet = false);
      virtual Pin * addPin(Pin * pin, bool quiet = false);
      virtual bool removePin(Pin * pin, bool quiet = false);

      virtual std::vector<Node*> upStreamNodes(bool sortForPins = false, std::vector<Node*> rootNodes = std::vector<Node*>());
      // temporary information around row and col
      virtual int row() const;
      virtual void setRow(int i);
      virtual int col() const;
      virtual void setCol(int i);

      virtual void setAlwaysShowDaisyChainPorts(bool state);

      QGraphicsWidget * mainWidget();
      QGraphicsWidget * pinsWidget();

    public slots:

      void onConnectionsChanged();
      void onBubbleEditRequested(FabricUI::GraphView::NodeBubble * bubble);
      
    signals:

      void selectionChanged(FabricUI::GraphView::Node *, bool);
      void collapsedStateChanged(FabricUI::GraphView::Node *, FabricUI::GraphView::Node::CollapseState);
      void positionChanged(FabricUI::GraphView::Node *, QPointF);
      void pinAdded(FabricUI::GraphView::Node *, Pin *);
      void pinRemoved(FabricUI::GraphView::Node *, Pin *);
      void doubleClicked(FabricUI::GraphView::Node *);
      void bubbleEditRequested(FabricUI::GraphView::Node * node);

    protected:

      void updatePinLayout();

      bool m_isBackDropNode;
      Graph * m_graph;
      std::string m_name;
      std::string m_title;
      NodeBubble * m_bubble;

      QColor m_colorA;
      QColor m_colorB;
      QColor m_titleColor;
      QColor m_fontColor;
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
      Qt::MouseButton m_dragButton;
      std::vector<Node *> m_nodesToMove;

      std::vector<Pin*> m_pins;
      CachingEffect * m_cache;
      int m_row;
      int m_col;
      bool m_alwaysShowDaisyChainPorts;
    };


  };

};

#endif // __UI_GraphView_Node__
