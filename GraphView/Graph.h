// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Graph__
#define __UI_GraphView_Graph__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsScene>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QMenu>

#include <FTL/StrRef.h>

#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/MouseGrabber.h>
#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/InfoOverlay.h>

#if QT_VERSION > 0x040602
# define DFG_QT_MIDDLE_MOUSE Qt::MiddleButton
#else
# define DFG_QT_MIDDLE_MOUSE Qt::MidButton
#endif

#define QSTRING_FROM_FTL_UTF8(s) QString::fromUtf8((s).data(), (s).size())
#define QSTRING_FROM_STL_UTF8(s) QString::fromUtf8((s).c_str(), (s).length())

namespace FabricUI
{

  namespace GraphView
  {

    class Graph : public QGraphicsWidget
    {
      Q_OBJECT

      friend class Controller;
      friend class RemoveNodeCommand;
      friend class AddConnectionCommand;
      friend class RemoveConnectionCommand;
      friend class Node;
      friend class MouseGrabber;

    public:

      Graph(
        QGraphicsItem * parent,
        const GraphConfig & config = GraphConfig()
        );
      virtual ~Graph() {}

      virtual void initialize();

      virtual const GraphConfig & config() const;
      QGraphicsWidget * itemGroup();
      virtual Controller * controller();
      virtual void setController(Controller * c);

      bool isEditable() const { return m_isEditable; }
      void setEditable(bool state) { m_isEditable = state; }

      MainPanel * mainPanel();
      const MainPanel * mainPanel() const;
      bool hasSidePanels() const;
      SidePanel * sidePanel(PortType portType);
      const SidePanel * sidePanel(PortType portType) const;

      // nodes
      virtual std::vector<Node *> nodes() const;
      virtual Node * node( FTL::StrRef name ) const;
      virtual Node * nodeFromPath( FTL::StrRef path ) const
        { return node( path ); }

      virtual std::vector<Node *> selectedNodes() const;
      void clearSelection() const;

      // ports
      virtual std::vector<Port *> ports() const;
      virtual Port * port(FTL::StrRef name) const;

      // connections
      virtual std::vector<Connection *> connections() const;
      virtual bool isConnected(const ConnectionTarget * target) const;
      virtual void updateColorForConnections(const ConnectionTarget * target) const;

      // hotkeys
      virtual void defineHotkey(Qt::Key key, Qt::KeyboardModifier modifiers, QString name);

      // context menus
      // menus are consumed by the graph, so they are destroyed after use.
      typedef QMenu* (*GraphContextMenuCallback)(Graph*, void*);
      typedef QMenu* (*NodeContextMenuCallback)(Node*, void*);
      typedef QMenu* (*PinContextMenuCallback)(Pin*, void*);
      typedef QMenu* (*ConnectionContextMenuCallback)(Connection*, void*);
      typedef QMenu* (*PortContextMenuCallback)(Port*, void*);
      typedef QMenu* (*SidePanelContextMenuCallback)(SidePanel*, void*);
      virtual void setGraphContextMenuCallback(GraphContextMenuCallback callback, void * userData = NULL);
      virtual void setNodeContextMenuCallback(NodeContextMenuCallback callback, void * userData = NULL);
      virtual void setPinContextMenuCallback(PinContextMenuCallback callback, void * userData = NULL);
      virtual void setConnectionContextMenuCallback(ConnectionContextMenuCallback callback, void * userData = NULL);
      virtual void setPortContextMenuCallback(PortContextMenuCallback callback, void * userData = NULL);
      virtual void setSidePanelContextMenuCallback(SidePanelContextMenuCallback callback, void * userData = NULL);
      virtual QMenu* getGraphContextMenu();
      virtual QMenu* getNodeContextMenu(Node * node);
      virtual QMenu* getPinContextMenu(Pin * pin);
      virtual QMenu* getConnectionContextMenu(Connection * connection);
      virtual QMenu* getPortContextMenu(Port * Port);
      virtual QMenu* getSidePanelContextMenu(SidePanel * sidePanel);

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      MouseGrabber * constructMouseGrabber(QPointF pos, ConnectionTarget * target, PortType portType);
      MouseGrabber * getMouseGrabber();

      // interaction
      virtual Node * addNode(Node * node, bool quiet = false);
      virtual Node * addNode(FTL::CStrRef name, FTL::CStrRef preset, bool quiet = false);
      virtual BackDropNode * addBackDropNode( FTL::CStrRef name, bool quiet = false );
      virtual bool removeNode(Node * node, bool quiet = false);
      virtual bool addPort(Port * port, bool quiet = false);
      virtual bool removePort(Port * port, bool quiet = false);
      virtual Connection * addConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet = false);
      virtual bool removeConnection(ConnectionTarget * src, ConnectionTarget * dst, bool quiet = false);
      virtual bool removeConnection(Connection * connection, bool quiet = false);
      virtual void resetMouseGrabber();

      void updateOverlays(float width, float height);
      void setupBackgroundOverlay(QPointF pos, QString filePath);
      void setCentralOverlayText(QString text);

    public slots:

      virtual bool pressHotkey(Qt::Key key, Qt::KeyboardModifier modifiers);
      virtual bool releaseHotkey(Qt::Key key, Qt::KeyboardModifier modifiers);
      void onNodeDoubleClicked(FabricUI::GraphView::Node * node, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
      void requestSidePanelInspect(
        FabricUI::GraphView::SidePanel *sidePanel
        );
      void onBubbleEditRequested(FabricUI::GraphView::Node * node);

    signals:

      void graphChanged(FabricUI::GraphView::Graph * graph, QString path);
      void nodeAdded(FabricUI::GraphView::Node * node);
      void nodeRemoved(FabricUI::GraphView::Node * node);
      void nodeSelected(FabricUI::GraphView::Node * node);
      void nodeDeselected(FabricUI::GraphView::Node * node);
      void nodeMoved(FabricUI::GraphView::Node * node, QPointF pos);
      void nodeInspectRequested(FabricUI::GraphView::Node *);
      void nodeEditRequested(FabricUI::GraphView::Node *);
      void sidePanelInspectRequested();
      void connectionAdded(FabricUI::GraphView::Connection * connection);
      void connectionRemoved(FabricUI::GraphView::Connection * connection);
      void hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString);
      void hotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString);
      void bubbleEditRequested(FabricUI::GraphView::Node * node);

    private:


      struct Hotkey
      {
        Qt::Key key;
        Qt::KeyboardModifier modifiers;

        Hotkey(Qt::Key key, Qt::KeyboardModifier modifiers)
        {
          this->key = key;
          this->modifiers = modifiers;
        }

        bool operator < (const Hotkey & other) const
        {
          if((int)key < (int)other.key)
            return true;
          if((int)key > (int)other.key)
            return false;
          return (int)modifiers < (int)other.modifiers;
        }
      };

      GraphConfig m_config;
      Controller * m_controller;
      std::vector<Node *> m_nodes;
      std::map<FTL::StrRef, size_t> m_nodeMap;
      std::vector<Connection *> m_connections;
      MouseGrabber * m_mouseGrabber;
      MainPanel * m_mainPanel;
      SidePanel * m_leftPanel;
      SidePanel * m_rightPanel;
      std::map<Hotkey, QString> m_hotkeys;
      GraphContextMenuCallback m_graphContextMenuCallback;
      NodeContextMenuCallback m_nodeContextMenuCallback;
      PinContextMenuCallback m_pinContextMenuCallback;
      ConnectionContextMenuCallback m_connectionContextMenuCallback;
      PortContextMenuCallback m_portContextMenuCallback;
      SidePanelContextMenuCallback m_sidePanelContextMenuCallback;
      void * m_graphContextMenuCallbackUD;
      void * m_nodeContextMenuCallbackUD;
      void * m_pinContextMenuCallbackUD;
      void * m_connectionContextMenuCallbackUD;
      void * m_portContextMenuCallbackUD;
      void * m_sidePanelContextMenuCallbackUD;
      InfoOverlay * m_centralOverlay;
      QString m_centralOverlayText;
      bool m_isEditable;
      QPointF m_overlayPos;
      QPixmap m_overlayPixmap;
      QGraphicsPixmapItem * m_overlayItem;
      double m_nodeZValue;
      double m_backdropZValue;
      double m_connectionZValue;

    };

  };

};

#endif // __UI_GraphView_Graph__
