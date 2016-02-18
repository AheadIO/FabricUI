// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

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

      friend class Graph;
      friend class NodeRectangle;
      friend class NodeBubble;
      friend class NodeHeaderButton;

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
      FTL::CStrRef titleSuffix() const
        { return m_titleSuffix; }
      void setTitleSuffix( FTL::CStrRef titleSuffix );
      void setTitleSuffixAsterisk() { setTitleSuffix(" *"); }
      void removeTitleSuffix() { setTitleSuffix(""); }
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

      virtual bool selected() const;

      virtual CollapseState collapsedState() const;
      virtual void setCollapsedState(CollapseState state);
      virtual void toggleCollapsedState();

      virtual QString error() const;
      virtual bool hasError() const;
      virtual void setError(QString text);
      virtual void clearError();

      QPointF graphPos() const
        { return topLeftToCentralPos( topLeftGraphPos() ); }
      void setGraphPos( QPointF pos, bool quiet = false );

      QPointF topLeftToCentralPos( QPointF pos ) const
      {
        QRectF rect = boundingRect();
        return QPointF(
          pos.x() + rect.width() * 0.5,
          pos.y() + rect.height() * 0.5
          );
      }
      QPointF centralPosToTopLeftPos( QPointF pos ) const
      {
        QRectF rect = boundingRect();
        return QPointF(pos.x() - rect.width() * 0.5, pos.y() - rect.height() * 0.5);
      }

      QPointF topLeftGraphPos() const
        { return pos(); }
      void setTopLeftGraphPos( QPointF pos, bool quiet = false );

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

      bool addPin( Pin * pin );
      bool removePin( Pin * pin );

      Pin *renamePin( FTL::StrRef oldName, FTL::StrRef newName );
      virtual void reorderPins(QStringList names);

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
      void doubleClicked(FabricUI::GraphView::Node *, Qt::MouseButton, Qt::KeyboardModifiers);
      void bubbleEditRequested(FabricUI::GraphView::Node * nod);

// [pzion 20150905] The version check causes link errors, but
// ommitting it doesn't seem to cause problems anyway...
// #if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
      void geometryChanged();
// #endif

    protected:

      void updateEffect();
      
#if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
      virtual void updateGeometry();
#endif

      void updatePinLayout();

      // used by NodeHeader / NodeHeaderButton
      bool onMousePress(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);
      bool onMouseMove(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);
      bool onMouseRelease(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);
      bool onMouseDoubleClicked(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);

      bool m_isBackDropNode;
      Graph * m_graph;
      std::string m_name;
      std::string m_title;
      std::string m_titleSuffix;
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
      QPointF m_mouseDownPos;
      std::vector<Node *> m_nodesToMove;

      std::vector<Pin*> m_pins;
      int m_row;
      int m_col;
      bool m_alwaysShowDaisyChainPorts;
    };


  };

};

#endif // __UI_GraphView_Node__
