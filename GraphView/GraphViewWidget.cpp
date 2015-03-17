// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "GraphViewWidget.h"

#include <QtGui/QPainter>

using namespace FabricUI::GraphView;

GraphViewWidget::GraphViewWidget(QWidget * parent, const GraphConfig & config, GraphFactory * factory, Graph * graph)
: QGraphicsView(parent)
{
  setRenderHint(QPainter::Antialiasing);
  setRenderHint(QPainter::HighQualityAntialiasing);
  setRenderHint(QPainter::TextAntialiasing);

  setStyleSheet( "QGraphicsView { border-style: none; }" );

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  setCacheMode(CacheBackground);
  setViewportUpdateMode(SmartViewportUpdate);

  m_scene = new QGraphicsScene();
  setScene(m_scene);

  m_graph = graph;
  if(!m_graph)
    m_graph = new Graph(NULL, config, factory);
  m_scene->addItem(m_graph);

  setAcceptDrops(true);
  setMouseTracking(true);
}

Graph * GraphViewWidget::graph()
{
  return m_graph;
}

const Graph * GraphViewWidget::graph() const
{
  return m_graph;
}

void GraphViewWidget::resizeEvent(QResizeEvent * event)
{
  setSceneRect(0, 0, event->size().width(), event->size().height());
  m_graph->setGeometry(0, 0, event->size().width(), event->size().height());
}

void GraphViewWidget::dragMoveEvent(QDragMoveEvent *event)
{
  event->accept();
}

void GraphViewWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("text/plain"))
  {
    event->acceptProposedAction();
  }
}

void GraphViewWidget::dropEvent(QDropEvent *event)
{
  m_lastEventPos = event->pos();

  // event->mimeData()->text()
  QString preset = event->mimeData()->text();

  graph()->controller()->beginInteraction();

  QPointF pos(event->pos().x(), event->pos().y());
  pos = graph()->itemGroup()->mapFromScene(pos);

  Node * node = graph()->controller()->addNodeFromPreset(preset, pos);

  graph()->controller()->endInteraction();

  if(node)
    event->acceptProposedAction();
}

void GraphViewWidget::mouseMoveEvent(QMouseEvent * event)
{
  m_lastEventPos = event->pos();
  QGraphicsView::mouseMoveEvent(event);
}

void GraphViewWidget::keyPressEvent(QKeyEvent * event)
{
  if(graph()->pressHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    QGraphicsView::keyPressEvent(event);
}

void GraphViewWidget::keyReleaseEvent(QKeyEvent * event)
{
  if(graph()->releaseHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    QGraphicsView::keyPressEvent(event);
}

QPoint GraphViewWidget::lastEventPos() const
{
  return m_lastEventPos;
}

bool GraphViewWidget::focusNextPrevChild(bool next)
{
  // avoid focus switching
  return false;
}

/*

    ################################################
    ## Graph
    def getGraph(self):
        return self.__graph

    def popSubGraph(self):
        self.__graph.popSubGraph()

    def getGraphPath(self):
        return self.__graph.getGraphPath()

    def frameSelectedNodes(self):
        self.__graph.frameSelectedNodes()

    def frameAllNodes(self):
        self.__graph.frameAllNodes()

    def deleteSelectedNodes(self):
        self.__graph.deleteSelectedNodes()


    ################################################
    ## Function

    def openEditFunctionDialog(self, nodePath):
        DFGFunctionEditorDockWidget.showWidget(self.controller, nodePath)

    ################################################
    ## Ports

    def openAddPortDialog(self, graphPath, portType=None):
        addPortDialog = AddPortDialog(
                self,
                self.controller,
                executablePath=graphPath,
                portType=portType
            )
        addPortDialog.show()

    ################################################
    ## Events

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.MouseButton.RightButton:
            def __getGraphItem(graphicItem):
                if isinstance(graphicItem, Node):
                    return graphicItem
                if isinstance(graphicItem, Port):
                    return graphicItem
                if isinstance(graphicItem, ProxyPort):
                    return graphicItem
                elif isinstance(graphicItem, MainPanel):
                    return graphicItem
                elif(isinstance(graphicItem, QtGui.QGraphicsTextItem) or
                     isinstance(graphicItem, NodeLabel) or
                     isinstance(graphicItem, NodeHeader) or
                     isinstance(graphicItem, PortLabel) or
                     isinstance(graphicItem, PortFromPortPath)):
                    return __getGraphItem(graphicItem.parentItem())
                return None
            pos = self.getGraph().mapToScene(event.pos())
            graphicItem = __getGraphItem(self.itemAt(int(pos.x()), int(pos.y())))
            contextMenu = QtGui.QMenu(self.__dfgEditor)

            if isinstance(graphicItem, MainPanel):
                def newGraphNode():
                    ContextualNewNodeWidget(self, self.controller, self.getGraph(), 'graph', pos)
                contextMenu.addAction("New Graph Node").triggered.connect(newGraphNode)
                def newFunctionNode():
                    ContextualNewNodeWidget(self, self.controller, self.getGraph(), 'function', pos)
                contextMenu.addAction("New Function Node").triggered.connect(newFunctionNode)

            elif isinstance(graphicItem, ProxyPort):
                connectionPointType = graphicItem.getSidePanel().getConnectionPortType()
                if connectionPointType == 'In':
                    portType = 'Out'
                elif connectionPointType == 'Out':
                    portType = 'In'
                else:
                    portType = connectionPointType
                def addPort():
                    self.openAddPortDialog(
                        graphPath=self.__graph.getGraphPath(),
                        portType=portType
                        )
                contextMenu.addAction("Add Port").triggered.connect(addPort)

            elif isinstance(graphicItem, Port):
                def editPort():
                    print "EditPort"
                contextMenu.addAction("Edit Port").triggered.connect(editPort)
                def removePort():
                    self.controller.removePort(
                        executablePath=self.__graph.getGraphPath(),
                        graphPath=self.__graph.getGraphPath(),
                        portName=graphicItem.getName()
                        )
                contextMenu.addAction("Remove Port").triggered.connect(removePort)

            elif isinstance(graphicItem, Node):
                def newGraph():
                    ContextualNewNodeWidget(self, self.controller, self.getGraph(), 'graph', pos)
                contextMenu.addAction("New Graph Node").triggered.connect(newGraph)
            contextMenu.popup(event.globalPos())

        else:
            super(GraphView, self).mousePressEvent(event)

    def dragEnterEvent(self, event):
        textParts = event.mimeData().text().split(':')
        if textParts[0] == 'DFGNode':
            event.accept()
        else:
            event.setDropAction(QtCore.Qt.IgnoreAction)
            super(GraphView, self).dragEnterEvent(event)
            #event.setDropAction(QtCore.Qt.IgnoreAction)

    def dragMoveEvent(self, event):
        super(GraphView, self).dragMoveEvent(event)
        event.accept()

    def dropEvent(self, event):
        textParts = event.mimeData().text().split(':')
        if textParts[0] == 'DFGNode':
            presetPath = textParts[1]

            # Add a node to the graph at the given position.
            dropPosition = self.__graph.mapToItem(self.__graph.itemGroup(), event.pos())
            self.controller.addNodeFromPreset(
                graphPath=self.__graph.getGraphPath(), 
                presetPath=presetPath, 
                graphPos=dropPosition
            )

            event.acceptProposedAction()
        else:
            super(GraphView, self).dropEvent(event)

    def closeEvent(self, event):
        return super(GraphView, self).closeEvent(event)

    def saveExecutable(self):
        self.controller.saveExecutable(executablePath=self.__graph.getGraphPath())

*/
