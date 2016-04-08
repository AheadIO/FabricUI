// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <iostream>
#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QMessageBox>

#include <FTL/JSONEnc.h>
#include <FTL/Str.h>
#include <FTL/Math.h>
#include <FTL/MapCharSingle.h>

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphRelaxer.h>

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGErrorsWidget.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/DFGNotificationRouter.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGUIUtil.h>
#include <FabricUI/DFG/DFGWidget.h>

#include <sstream>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGController::DFGController(
  GraphView::Graph * graph,
  DFGWidget *dfgWidget,
  FabricCore::Client &client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  DFGUICmdHandler *cmdHandler,
  bool overTakeBindingNotifications
  )
  : GraphView::Controller(graph)
  , m_notificationTimer( new QTimer( this ) )
  , m_dfgWidget( dfgWidget )
  , m_client(client)
  , m_manager(manager)
  , m_cmdHandler( cmdHandler )
  , m_router(0)
  , m_logFunc(0)
  , m_overTakeBindingNotifications( overTakeBindingNotifications )
  , m_updateSignalBlockCount( 0 )
  , m_varsChangedPending( false )
  , m_argsChangedPending( false )
  , m_argValuesChangedPending( false )
  , m_defaultValuesChangedPending( false )
  , m_topoDirtyPending( false )
  , m_dirtyPending( false )
{
  m_notificationTimer->setSingleShot( true );
  connect(
    m_notificationTimer, SIGNAL(timeout()),
    this, SLOT(onNotificationTimer())
    );

  m_router = NULL;
  m_logFunc = NULL;
  m_presetDictsUpToDate = false;

  QObject::connect(this, SIGNAL(topoDirty()), this, SLOT(onTopoDirty()));
  QObject::connect(this, SIGNAL(varsChanged()), this, SLOT(onVariablesChanged()));
}

DFGController::~DFGController()
{
}

void DFGController::setHostBindingExec(
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
  )
{
  m_host = host;

  setBindingExec( binding, execPath, exec );

  emit hostChanged();
}

void DFGController::setBindingExec(
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
  )
{
  if ( m_binding.isValid() )
    m_bindingNotifier.clear();

  m_binding = binding;

  if ( m_binding.isValid() )
  {
    m_bindingNotifier = DFGBindingNotifier::Create( m_binding );

    connect(
      m_bindingNotifier.data(),
      SIGNAL(dirty()),
      this,
      SLOT(onBindingDirty())
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(topoDirty()),
      this,
      SLOT(onBindingTopoDirty())
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argInserted(unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingArgInserted(unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argTypeChanged(unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingArgTypeChanged(unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argRemoved(unsigned, FTL::CStrRef)),
      this,
      SLOT(onBindingArgRemoved(unsigned, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argsReordered(FTL::ArrayRef<unsigned>)),
      this,
      SLOT(onBindingArgsReordered(FTL::ArrayRef<unsigned>))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(argValueChanged(unsigned, FTL::CStrRef)),
      this,
      SLOT(onBindingArgValueChanged(unsigned, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(varInserted(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingVarInserted(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_bindingNotifier.data(),
      SIGNAL(varRemoved(FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onBindingVarRemoved(FTL::CStrRef, FTL::CStrRef))
      );
  }

  setExec( execPath, exec );

  updateErrors();

  emit bindingChanged( m_binding );
}

void DFGController::setExec(
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
  )
{
  m_execPath = execPath;
  m_exec = exec;

  m_presetDictsUpToDate = false;

  emit execChanged();
}

void DFGController::refreshExec()
{
  emit execChanged();
}

DFGNotificationRouter * DFGController::getRouter()
{
  return m_router;
}

void DFGController::setRouter(DFGNotificationRouter * router)
{
  if ( m_router )
  {
    QObject::disconnect(
      this, SIGNAL(execChanged()),
      m_router, SLOT(onExecChanged())
      );
  }

  m_router = router;

  if ( m_router )
  {
    QObject::connect(
      this, SIGNAL(execChanged()),
      m_router, SLOT(onExecChanged())
      );
  }
}

bool DFGController::validPresetSplit() const
{
  if(isViewingRootGraph())
    return true;

  if(!m_exec.isValid())
    return true;

  if(!m_exec.editWouldSplitFromPreset())
    return true;

  // QMessageBox msgBox;
  // msgBox.setText( "You are about to split this node from the preset. Are you sure?" );
  // msgBox.setInformativeText( "The node will no longer be referencing the preset." );
  // msgBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
  // msgBox.setDefaultButton( QMessageBox::Cancel );
  // switch(msgBox.exec())
  // {
  //   case QMessageBox::Ok:
  //     return true;
  //   case QMessageBox::Cancel:
  //     return false;
  // }

  return false;
}

bool DFGController::gvcDoRemoveNodes(
  FTL::ArrayRef<GraphView::Node *> nodes
  )
{
  if ( !nodes.empty() )
  {
    QStringList nodeNames;
    nodeNames.reserve( nodes.size() );
    for ( unsigned i = 0; i < nodes.size(); ++i )
    {
      FTL::CStrRef nodeName = nodes[i]->name();
      nodeNames.push_back(
        QString::fromUtf8( nodeName.data(), nodeName.size() )
        );
    }

    cmdRemoveNodes( nodeNames );
  }
  return true;
}

void DFGController::cmdAddBackDrop(
  QString title,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoAddBackDrop(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    title,
    pos
    );
}

QString DFGController::cmdEditNode(
  QString oldName,
  QString desiredNewName,
  QString nodeMetadata,
  QString execMetadata
  )
{
  if(!validPresetSplit())
    return oldName;

  return m_cmdHandler->dfgDoEditNode(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    oldName,
    desiredNewName,
    nodeMetadata,
    execMetadata
    );
}

void DFGController::cmdSetNodeComment(
  QString nodeName,
  QString comment
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoSetNodeComment(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeName,
    comment
    );
}

void DFGController::setNodeCommentExpanded(
  QString nodeName,
  bool expanded
  )
{
  m_exec.setNodeMetadata(
    nodeName.toUtf8().constData(),
    "uiCommentExpanded",
    expanded? "true": "",
    false,
    false
    );
}

bool DFGController::gvcDoAddInstFromPreset(
  QString presetPath,
  QPointF pos
  )
{
  cmdAddInstFromPreset(
    presetPath,
    pos
    );

  return true;
}

void DFGController::gvcDoAddPort(
  QString desiredPortName,
  GraphView::PortType portType,
  QString typeSpec,
  GraphView::ConnectionTarget *connectWith,
  QString extDep,
  QString metaData
  )
{
  FabricCore::DFGPortType dfgPortType;
  switch ( portType )
  {
    case GraphView::PortType_Input:
      dfgPortType = FabricCore::DFGPortType_Out;
      break;
    
    case GraphView::PortType_Output:
      dfgPortType = FabricCore::DFGPortType_In;
      break;
    
    case GraphView::PortType_IO:
      dfgPortType = FabricCore::DFGPortType_IO;
      break;

    default:
      // [andrew 20150730] shouldn't be possible but needed to prevent
      // compiler warning
      assert( false );
      dfgPortType = FabricCore::DFGPortType_In;
      break;
  }

  std::string connectWithPath;
  if ( connectWith )
    connectWithPath = connectWith->path();
  cmdAddPort(
    desiredPortName,
    dfgPortType,
    typeSpec,
    QString::fromUtf8( connectWithPath.data(), connectWithPath.size() ),
    extDep,
    metaData
    );
}

void DFGController::gvcDoSetNodeCommentExpanded(
  GraphView::Node *node,
  bool expanded
  )
{
  FTL::CStrRef nodeName = node->name();
  setNodeCommentExpanded(
    QString::fromUtf8( nodeName.data(), nodeName.size() ),
    expanded
    );
}

QString DFGController::cmdRenameExecPort(
  QString oldName,
  QString desiredNewName
  )
{
  if(!validPresetSplit())
    return QString();

  QString result = m_cmdHandler->dfgDoRenamePort(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    oldName,
    desiredNewName
    );

  if ( !result.isEmpty() )
    emit argsChanged();

  return result;
}

bool DFGController::gvcDoAddConnection(
  GraphView::ConnectionTarget * src,
  GraphView::ConnectionTarget * dst
  )
{
  std::string srcPath;
  if(src->targetType() == GraphView::TargetType_Pin)
    srcPath = ((GraphView::Pin*)src)->path();
  else if(src->targetType() == GraphView::TargetType_Port)
    srcPath = ((GraphView::Port*)src)->path();

  std::string dstPath;
  if(dst->targetType() == GraphView::TargetType_Pin)
    dstPath = ((GraphView::Pin*)dst)->path();
  else if(dst->targetType() == GraphView::TargetType_Port)
    dstPath = ((GraphView::Port*)dst)->path();

  cmdConnect(
    QString::fromUtf8( srcPath.data(), srcPath.size() ),
    QString::fromUtf8( dstPath.data(), dstPath.size() )
    );
  
  return true;
}

bool DFGController::gvcDoRemoveConnection(
  GraphView::ConnectionTarget * src,
  GraphView::ConnectionTarget * dst
  )
{
  std::string srcPath;
  if(src->targetType() == GraphView::TargetType_Pin)
    srcPath = ((GraphView::Pin*)src)->path();
  else if(src->targetType() == GraphView::TargetType_Port)
    srcPath = ((GraphView::Port*)src)->path();

  std::string dstPath;
  if(dst->targetType() == GraphView::TargetType_Pin)
    dstPath = ((GraphView::Pin*)dst)->path();
  else if(dst->targetType() == GraphView::TargetType_Port)
    dstPath = ((GraphView::Port*)dst)->path();

  cmdDisconnect(
    QString::fromUtf8( srcPath.data(), srcPath.size() ),
    QString::fromUtf8( dstPath.data(), dstPath.size() )
    );

  return true;
}

void DFGController::cmdSetCode( QString code )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoSetCode(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    code
    );
}

QString DFGController::reloadCode()
{
  if(!validPresetSplit())
    return QString();

  FabricCore::DFGExec &func = getExec();
  if ( func.getType() != FabricCore::DFGExecType_Func )
    return QString();

  char const * filePath = func.getImportPathname();
  FILE * file = fopen(filePath, "rb");
  if(file)
  {
    fseek( file, 0, SEEK_END );
    int fileSize = ftell( file );
    rewind( file );

    char * buffer = (char*) malloc(fileSize + 1);
    buffer[fileSize] = '\0';

    fread(buffer, 1, fileSize, file);

    fclose(file);

    std::string json = buffer;
    free(buffer);

    try
    {
      FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(json.c_str());
      if(jsonVar.isDict())
      {
        const FabricCore::Variant * klCodeVar = jsonVar.getDictValue("code");
        if(klCodeVar)
        {
          if(klCodeVar->isString())
          {
            QString klCode = QString::fromUtf8(
              klCodeVar->getString_cstr()
              );
            cmdSetCode( klCode );
            return klCode;
          }
        }
      }
    }
    catch(FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
    }
  }    
  return QString();
}

bool DFGController::zoomCanvas(
  float zoom
  )
{
  if ( FTL::isnan( zoom ) || FTL::isinf( zoom ) )
    return false;

  try
  {
    FabricCore::DFGExec &exec = getExec();

    std::string json;
    {
      FTL::JSONEnc<> enc( json );
      FTL::JSONObjectEnc<> objEnc( enc );
      {
        FTL::JSONEnc<> zoomEnc( objEnc, FTL_STR("value") );
        FTL::JSONFloat64Enc<> zoomS32Enc( zoomEnc, zoom );
      }
    }

    exec.setMetadata("uiGraphZoom", json.c_str(), false, false);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

bool DFGController::panCanvas(QPointF pan)
{
  try
  {
    FabricCore::DFGExec &exec = getExec();

    std::string json;
    {
      FTL::JSONEnc<> enc( json );
      FTL::JSONObjectEnc<> objEnc( enc );
      {
        FTL::JSONEnc<> xEnc( objEnc, FTL_STR("x") );
        FTL::JSONFloat64Enc<> xS32Enc( xEnc, pan.x() );
      }
      {
        FTL::JSONEnc<> yEnc( objEnc, FTL_STR("y") );
        FTL::JSONFloat64Enc<> yS32Enc( yEnc, pan.y() );
      }
    }

    exec.setMetadata("uiGraphPan", json.c_str(), false, false);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

bool DFGController::relaxNodes(QStringList paths)
{
  if(paths.length() == 0)
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
    for(unsigned int i=0;i<nodes.size();i++)
    {
      if(nodes[i]->type() != GraphView::QGraphicsItemType_Node)
        continue;
      paths.append(nodes[i]->name().c_str());
    }
  }

  std::vector<GraphView::Node*> allNodes = graph()->nodes();
  for(unsigned int i=0;i<allNodes.size();i++)
  {
    allNodes[i]->setRow(-1);
    allNodes[i]->setCol(-1);
  }

  std::vector<GraphView::Node*> rootNodes;
  for(int i=0;i<paths.length();i++)
  {
    if ( GraphView::Node *uiNode = graph()->node( paths[i] ) )
      rootNodes.push_back( uiNode );
  }

  if(rootNodes.size() == 0)
    return false;

  std::vector<GraphView::Node*> nodes = rootNodes[0]->upStreamNodes(true, rootNodes);
  if(nodes.size() <= 1)
    return false;

  GraphView::GraphRelaxer relaxer(graph()->config());
  for(unsigned int i=0;i<nodes.size();i++)
  {
    QRectF rect = nodes[i]->boundingRect();
    relaxer.addNode(nodes[i]->name().c_str(), nodes[i]->topLeftGraphPos(), nodes[i]->col(), nodes[i]->row(), rect.width(), rect.height());
  }

  std::vector<GraphView::Connection*> connections = graph()->connections();
  for(unsigned int i=0;i<connections.size();i++)
  {
    GraphView::ConnectionTarget * src = connections[i]->src();
    if(src->targetType() != GraphView::TargetType_Pin)
      continue;

    GraphView::ConnectionTarget * dst = connections[i]->dst();
    if(dst->targetType() != GraphView::TargetType_Pin)
      continue;

    GraphView::Pin * srcPin = (GraphView::Pin *)src;
    GraphView::Pin * dstPin = (GraphView::Pin *)dst;

    GraphView::Node * srcNode = srcPin->node();
    GraphView::Node * dstNode = dstPin->node();

    relaxer.addSpring(srcNode->name().c_str(), dstNode->name().c_str());
  }

  relaxer.relax(50);

  QStringList nodeNames;
  nodeNames.reserve( relaxer.numNodes() );
  QList<QPointF> newTopLeftPoss;
  newTopLeftPoss.reserve( relaxer.numNodes() );
  for ( unsigned i=0; i<relaxer.numNodes(); i++ )
  {
    FTL::CStrRef nodeName = relaxer.getName(i);
    nodeNames.push_back( QString::fromUtf8( nodeName.data(), nodeName.size() ) );
    newTopLeftPoss.push_back( relaxer.getPos(i) );
  }

  cmdMoveNodes( nodeNames, newTopLeftPoss );

  return true;
}

bool DFGController::setNodeColor(
  const char * nodeName,
  const char * key,
  QColor color
  )
{
  if(!validPresetSplit())
    return false;

  try
  {
    std::string uiNodeColorString;
    {
      FTL::JSONEnc<> enc( uiNodeColorString );
      FTL::JSONObjectEnc<> objEnc( enc );
      {
        FTL::JSONEnc<> rEnc( objEnc, FTL_STR("r") );
        FTL::JSONSInt32Enc<> rS32Enc( rEnc, color.red() );
      }
      {
        FTL::JSONEnc<> gEnc( objEnc, FTL_STR("g") );
        FTL::JSONSInt32Enc<> gS32Enc( gEnc, color.green() );
      }
      {
        FTL::JSONEnc<> bEnc( objEnc, FTL_STR("b") );
        FTL::JSONSInt32Enc<> bS32Enc( bEnc, color.blue() );
      }
    }

    FabricCore::DFGExec &exec = getExec();
    exec.setNodeMetadata(
      nodeName,
      key,
      uiNodeColorString.c_str(),
      false, 
      true
      );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

/// Sets the collapse state of the selected node.
/// Saves it in the node preferences
void DFGController::setSelectedNodeCollapseState(int collapsedState) {
  // Call the parent function
  collapseSelectedNodes(collapsedState);
  // Now, set the collapse state in the node preferences
  FabricCore::Variant collapsedStateVar = FabricCore::Variant::CreateSInt32(collapsedState);
  FabricCore::DFGExec &exec = getExec();
  const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
  for(unsigned int i=0;i<nodes.size();i++)
  {
    if(nodes[i]->type() != GraphView::QGraphicsItemType_Node)
      continue;
    exec.setNodeMetadata(nodes[i]->name().c_str(), "uiCollapsedState", collapsedStateVar.getJSONEncoding().getStringData(), false, false);
  }
} 

std::string DFGController::copy()
{
  std::string json;
  try
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();

    if (nodes.size() == 0)
    {
      QApplication::clipboard()->clear();
      return "";
    }

    std::vector<std::string> pathStrs;
    pathStrs.reserve( nodes.size() );

    for ( size_t i = 0; i < nodes.size(); ++i )
    {
      if ( nodes[i]->type() != GraphView::QGraphicsItemType_Node )
        continue;
      pathStrs.push_back( nodes[i]->name() );
    }

    std::vector<char const *> pathCStrs;
    pathCStrs.reserve( pathStrs.size() );
    for ( size_t i = 0; i < pathStrs.size(); ++i )
      pathCStrs.push_back( pathStrs[i].c_str() );

    json =
      m_exec.exportNodesJSON(
        pathCStrs.size(),
        &pathCStrs[0]
        ).getCString();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( json.c_str() );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return json;
}

void DFGController::cmdCut()
{
  if(!validPresetSplit())
    return;

  try
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();

    if (nodes.size() == 0)
    {
      QApplication::clipboard()->clear();
      return;
    }

    std::vector<std::string> pathStrs;
    pathStrs.reserve( nodes.size() );

    for ( size_t i = 0; i < nodes.size(); ++i )
    {
      if ( nodes[i]->type() != GraphView::QGraphicsItemType_Node )
        continue;
      pathStrs.push_back( nodes[i]->name() );
    }

    std::vector<char const *> pathCStrs;
    pathCStrs.reserve( pathStrs.size() );
    for ( size_t i = 0; i < pathStrs.size(); ++i )
      pathCStrs.push_back( pathStrs[i].c_str() );

    std::string json =
      m_exec.exportNodesJSON(
        pathCStrs.size(),
        &pathCStrs[0]
        ).getCString();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( json.c_str() );

    QStringList paths;
    paths.reserve( pathStrs.size() );
    for ( size_t i = 0; i < pathStrs.size(); ++i )
      paths.push_back(
        QString::fromUtf8( pathStrs[i].data(), pathStrs[i].size() )
        );

    cmdRemoveNodes( paths );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

void DFGController::cmdPaste()
{
  if(!validPresetSplit())
    return;

  try
  {
    QClipboard *clipboard = QApplication::clipboard();
    if (!clipboard->text().isEmpty())
    {
      // calculate the position where the nodes will get pasted.
      QGraphicsView *graphicsView = graph()->scene()->views()[0];
      QPointF pos = graphicsView->mapToScene(graphicsView->mapFromGlobal(QCursor::pos()));
      float   zoom = graph()->mainPanel()->canvasZoom();
      QPointF pan  = graph()->mainPanel()->canvasPan();
      pos -= pan;
      if (zoom != 0)  pos /= zoom;

      // paste.
      QList<QString> pastedNodes =
        m_cmdHandler->dfgDoPaste(
          getBinding(),
          getExecPath_QS(),
          getExec(),
          clipboard->text().toUtf8().constData(),
          pos
          );

      graph()->clearSelection();
      for ( int i = 0; i < pastedNodes.size(); ++i )
      {
        if ( FabricUI::GraphView::Node *node = graph()->node( pastedNodes[i] ) )
          node->setSelected( true );
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

bool DFGController::reloadExtensionDependencies(char const * path)
{
  FabricCore::DFGExec exec = getExec();
  FTL::StrRef pathRef(path);
  if(pathRef.size() > 0)
    exec = exec.getSubExec(path);

  if(!exec.isValid())
    return false;

  for(unsigned int i = 0; i < exec.getExtDepCount(); i++ )
  {
    char const * ext = exec.getExtDepName(i);
    std::string version = exec.getExtDepVersion(i).getCString();
    try
    {
      m_client.loadExtension(ext, version.c_str(), true);
    }
    catch(FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
      return false;
    }
  }

  execute();

  return true;
}

void DFGController::onTopoDirty()
{
  updateErrors();
  updateNodeErrors();
}

void DFGController::updateErrors()
{
  m_dfgWidget->getErrorsWidget()->onErrorsMayHaveChanged();
}

void DFGController::updateNodeErrors()
{
  // [pzion 20160209] This will force the Core to ensure errors are up-to-date
  (void)m_binding.hasRecursiveConnectedErrors();

  try
  {
    GraphView::Graph *uiGraph = 0;
    if ( m_exec.getType() == FabricCore::DFGExecType_Graph )
      uiGraph = graph();
    if ( uiGraph )
    {
      unsigned nodeCount = m_exec.getNodeCount();
      for(size_t j=0;j<nodeCount;j++)
      {
        char const *nodeName = m_exec.getNodeName(j);

        GraphView::Node *uiNode = uiGraph->nodeFromPath( nodeName );
        if ( !uiNode )
          continue;

        uiNode->clearError();
      }
    }

    FabricCore::String errorsJSON = m_exec.getErrors( true );
    FTL::CStrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getSize() );
    FTL::JSONStrWithLoc strWithLoc( errorsJSONStr );
    FTL::OwnedPtr<FTL::JSONArray> errorsJSONArray(
      FTL::JSONValue::Decode( strWithLoc )->cast<FTL::JSONArray>()
      );
    unsigned errorCount = errorsJSONArray->size();
    for(unsigned i=0;i<errorCount;i++)
    {
      FTL::JSONObject const *error = errorsJSONArray->getObject( i );

      FTL::CStrRef execPath = error->getString( FTL_STR("execPath") );
      FTL::CStrRef nodeName = error->getStringOrEmpty( FTL_STR("nodeName") );
      FTL::CStrRef desc = error->getString( FTL_STR("desc") );
      int32_t line = error->getSInt32Or( FTL_STR("line"), -1 );
      int32_t column = error->getSInt32Or( FTL_STR("column"), -1 );

      std::stringstream prefixedError;
      prefixedError << m_execPath;
      prefixedError << " : ";
      if ( !execPath.empty() )
        prefixedError << execPath;
      if ( !execPath.empty() && !nodeName.empty() )
        prefixedError << '.';
      if ( !nodeName.empty() )
        prefixedError << nodeName;
      if ( line != -1 )
      {
        prefixedError << ':';
        prefixedError << line;
        if ( column != -1 )
        {
          prefixedError << ':';
          prefixedError << column;
        }
      }
      prefixedError << " : ";
      prefixedError << desc;
      logError( prefixedError.str().c_str() );

      if ( uiGraph )
      {
        FTL::StrRef rootNodeName;
        if ( !execPath.empty() )
          rootNodeName = execPath.split('.').first;
        else
          rootNodeName = nodeName;
        if ( rootNodeName.empty() )
          continue;

        if ( GraphView::Node *uiNode = uiGraph->nodeFromPath( rootNodeName ) )
          uiNode->setError( QString::fromUtf8( desc.data(), desc.size() ) );
      }
    }
  }
  catch ( FTL::JSONException e )
  {
    std::cout
      << "Caught exception: "
      << e.getDesc()
      << "\n";
  }

  // [pzion 20150701] Upgrade old backdrops scheme
  static bool upgradingBackDrops = false;
  if ( !upgradingBackDrops )
  {
    upgradingBackDrops = true;
    
    char const * uiBackDropsCStr = m_exec.getMetadata( "uiBackDrops" );
    std::string uiBackDrops;
    if(uiBackDropsCStr != NULL)
      uiBackDrops = uiBackDropsCStr;

    if ( !uiBackDrops.empty() )
    {
      std::pair<FTL::StrRef, FTL::CStrRef> split = FTL::CStrRef( uiBackDrops ).split(',');
      while ( !split.first.empty() )
      {
        std::string uiBackDropKey = split.first;
        try
        {
          char const *nodeName = m_exec.addUser( uiBackDropKey.c_str() );

          FTL::CStrRef uiBackDrop = m_exec.getMetadata( uiBackDropKey.c_str() );
          if ( !uiBackDrop.empty() )
          {
            FTL::JSONStrWithLoc swl( uiBackDrop );
            FTL::OwnedPtr<FTL::JSONObject> jo(
              FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
              );

            m_exec.setNodeMetadata(
              nodeName,
              "uiTitle",
              jo->getString( FTL_STR("title") ).c_str(),
              false,
              true
              );

            FTL::JSONObject const *posJO =
              jo->getObject( FTL_STR("pos") );
            std::string posStr = posJO->encode();
            m_exec.setNodeMetadata(
              nodeName, "uiGraphPos", posStr.c_str(), false, false
              );

            FTL::JSONObject const *sizeJO =
              jo->getObject( FTL_STR("size") );
            std::string sizeStr;
            {
              FTL::JSONEnc<> sizeEnc( sizeStr );
              FTL::JSONObjectEnc<> sizeObjEnc( sizeEnc );
              {
                FTL::JSONEnc<> widthEnc( sizeObjEnc, FTL_STR("w") );
                FTL::JSONFloat64Enc<> widthF64Enc(
                  widthEnc, sizeJO->getFloat64( FTL_STR("width") )
                  );
              }
              {
                FTL::JSONEnc<> heightEnc( sizeObjEnc, FTL_STR("h") );
                FTL::JSONFloat64Enc<> heightF64Enc(
                  heightEnc, sizeJO->getFloat64( FTL_STR("height") )
                  );
              }
            }
            m_exec.setNodeMetadata(
              nodeName, "uiGraphSize", sizeStr.c_str(), false, false
              );

            FTL::JSONObject const *colorJO =
              jo->getObject( FTL_STR("color") );
            std::string colorStr = colorJO->encode();
            m_exec.setNodeMetadata(
              nodeName, "uiNodeColor", colorStr.c_str(), false, true
              );

            m_exec.setMetadata( uiBackDropKey.c_str(), "", false, false );
          }
        }
        catch ( ... )
        {
        }
        split = split.second.split(',');
      }
  
      m_exec.setMetadata( "uiBackDrops", "", false, false );
    }
    upgradingBackDrops = false;
  }
}

void DFGController::log(const char * message)
{
  DFGLogWidget::log(message);
  if(m_logFunc)
    (*m_logFunc)(message);
}

void DFGController::logError(const char * message)
{
  std::string m = "Error: ";
  m += message;
  log(m.c_str());
}

void DFGController::setLogFunc(LogFunc func)
{
  m_logFunc = func;
}

void DFGController::execute()
{
  try
  {
    m_binding.execute();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

void DFGController::cmdSetArgValue(
  QString argName,
  FabricCore::RTVal const &value
  )
{
  if(!validPresetSplit())
    return;

  FabricCore::RTVal currentValue =
    m_binding.getArgValue( argName.toUtf8().constData() );
  if ( !currentValue
    || !currentValue.isExEQTo( value ) )
  {
    m_cmdHandler->dfgDoSetArgValue(
      m_binding,
      argName,
      value.copy()
      );
  }
}

void DFGController::cmdSetRefVarPath(
  FabricCore::DFGBinding &binding,
  QString execPath,
  FabricCore::DFGExec &exec,
  QString refName,
  QString varPath
  )
{
  if(!validPresetSplit())
    return;

  FTL::CStrRef currentVarPath =
    exec.getRefVarPath( refName.toUtf8().constData() );
  if ( QString::fromUtf8( currentVarPath.c_str() ) != varPath )
  {
    m_cmdHandler->dfgDoSetRefVarPath(
      binding,
      execPath,
      exec,
      refName,
      varPath
      );
  }
}

void DFGController::cmdReorderPorts(
  FabricCore::DFGBinding &binding,
  QString execPath,
  FabricCore::DFGExec &exec,
  QList<int> indices
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoReorderPorts(
    binding,
    execPath,
    exec,
    indices
    );
}

void DFGController::cmdSetExtDeps(
  QStringList nameAndVers
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoSetExtDeps(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nameAndVers
    );
}

void DFGController::cmdSplitFromPreset()
{
  m_cmdHandler->dfgDoSplitFromPreset(
    getBinding(),
    getExecPath_QS(),
    getExec()
    );
}

bool DFGController::canConnectTo(
  char const *pathA,
  char const *pathB,
  std::string &failureReason
  )
{
  try
  {
    FabricCore::DFGStringResult result =
      getExec().canConnectTo( pathA, pathB, "\n" );
    char const *resultData;
    uint32_t resultSize;
    result.getStringDataAndLength( resultData, resultSize );
    if ( resultSize > 0 )
    {
      failureReason = std::string( resultData, resultSize );
      return false;
    }
    else return true;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

void DFGController::onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button)
{
  Controller::onNodeHeaderButtonTriggered(button);
  GraphView::Node * node = button->header()->node();  
  if(button->name() == "node_collapse")
  {
    setSelectedNodeCollapseState((int)node->collapsedState());
  }
  else if(button->name() == "node_edit")
  {
    emit nodeEditRequested(node);
  }
}

void DFGController::onVariablesChanged()
{
  m_presetDictsUpToDate = false;
  updatePresetPathDB();
}

void DFGController::onBindingDirty()
{
  emitDirty();
}

void DFGController::onBindingArgInserted(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef typeName
  )
{
  emitArgsChanged();
}

void DFGController::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newTypeName
  )
{
  emitArgsChanged();
}

void DFGController::onBindingTopoDirty()
{
  emitTopoDirty();
}

void DFGController::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name
  )
{
  emitArgsChanged();
}

void DFGController::onBindingArgsReordered(
  FTL::ArrayRef<unsigned> newOrder
  )
{
  emitArgsChanged();
}

void DFGController::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name
  )
{
  emitArgValuesChanged();
}

void DFGController::onBindingVarInserted(
  FTL::CStrRef varName,
  FTL::CStrRef varPath,
  FTL::CStrRef typeName,
  FTL::CStrRef extDep
  )
{
  emitVarsChanged();
}

void DFGController::onBindingVarRemoved(
  FTL::CStrRef varName,
  FTL::CStrRef varPath
  )
{
  emitVarsChanged();
}

QStringList DFGController::getPresetPathsFromSearch(char const * search, bool includePresets, bool includeNameSpaces)
{
  FTL::StrRef searchRef(search);
  if(searchRef.size() == 0)
    return QStringList();

  updatePresetPathDB();

  QStringList results;

  // [pzion 20150305] This is a little evil but avoids lots of copying

  std::string stdString = search;

  std::vector<FTL::StrRef> searchSplit;
  FTL::StrSplit<'.'>( stdString, searchSplit, true /*strict*/ );

  FTL::StrRemap< FTL::MapCharSingle<'.', '\0'> >( stdString );

  char const **cStrs = reinterpret_cast<char const **>(
    alloca( searchSplit.size() * sizeof( char const * ) )
    );
  for ( size_t i = 0; i < searchSplit.size(); ++i )
    cStrs[i] = searchSplit[i].data();

  if(includePresets)
  {
    SplitSearch::Matches matches =
      m_presetPathDict.search( searchSplit.size(), cStrs );

    if(matches.getSize() == 0)
      return results;
    std::vector<const char *> userDatas;
    userDatas.resize(matches.getSize());
    matches.getUserdatas(matches.getSize(), (const void**)&userDatas[0]);

    for(size_t i=0;i<userDatas.size();i++)
      results.push_back(userDatas[i]);
  }
  if(includeNameSpaces)
  {
    SplitSearch::Matches matches = m_presetNameSpaceDict.search( searchSplit.size(), cStrs );

    if(matches.getSize() == 0)
      return results;
    std::vector<const char *> userDatas;
    userDatas.resize(matches.getSize());
    matches.getUserdatas(matches.getSize(), (const void**)&userDatas[0]);

    for(size_t i=0;i<userDatas.size();i++)
      results.push_back(userDatas[i]);
  }

  return results;
}

void DFGController::updatePresetPathDB()
{
  if(m_presetDictsUpToDate || !m_host.isValid())
    return;
  m_presetDictsUpToDate = true;

  m_presetNameSpaceDict.clear();
  m_presetPathDict.clear();
  m_presetNameSpaceDictSTL.clear();
  m_presetPathDictSTL.clear();

  // insert fixed results for special nodes
  m_presetPathDictSTL.push_back("var");
  m_presetPathDictSTL.push_back("get");
  m_presetPathDictSTL.push_back("set");

  QStringList variables =
    getVariableWordsFromBinding(
      m_binding,
      m_execPath.c_str()
      );
  for(int i=0;i<variables.length();i++)
  {
    m_presetPathDictSTL.push_back("get." + std::string(variables[i].toUtf8().constData()));
    m_presetPathDictSTL.push_back("set." + std::string(variables[i].toUtf8().constData()));
  }

  std::vector<std::string> paths;
  paths.push_back("");

  for(size_t i=0;i<paths.size();i++)
  {
    std::string prefix = paths[i];
    if(prefix.length() > 0)
      prefix += ".";

    try
    {
      FabricCore::DFGStringResult jsonStr = m_host.getPresetDesc(paths[i].c_str());
      FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(jsonStr.getCString());
      const FabricCore::Variant * membersVar = jsonVar.getDictValue("members");
      for(FabricCore::Variant::DictIter memberIter(*membersVar); !memberIter.isDone(); memberIter.next())
      {
        std::string name = memberIter.getKey()->getStringData();
        const FabricCore::Variant * memberVar = memberIter.getValue();
        const FabricCore::Variant * objectTypeVar = memberVar->getDictValue("objectType");
        std::string objectType = objectTypeVar->getStringData();
        if(objectType == "Preset")
        {
          m_presetPathDictSTL.push_back(prefix+name);
        }
        else if(objectType == "NameSpace")
        {
          paths.push_back(prefix+name);
          m_presetNameSpaceDictSTL.push_back(prefix+name);
        }
      }
    }
    catch (FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
    }
  }

  for(size_t i=0;i<m_presetNameSpaceDictSTL.size();i++)
    m_presetNameSpaceDict.add(m_presetNameSpaceDictSTL[i].c_str(), '.', m_presetNameSpaceDictSTL[i].c_str());
  for(size_t i=0;i<m_presetPathDictSTL.size();i++)
    m_presetPathDict.add(m_presetPathDictSTL[i].c_str(), '.', m_presetPathDictSTL[i].c_str());
}

DFGNotificationRouter * DFGController::createRouter()
{
  return new DFGNotificationRouter( this );
}

QStringList DFGController::getVariableWordsFromBinding(FabricCore::DFGBinding & binding, FTL::CStrRef currentExecPath)
{
  QStringList words;

  FabricCore::DFGStringResult json =  binding.getVars();
  FTL::JSONStrWithLoc jsonStrWithLoc( json.getCString() );
  FTL::OwnedPtr<FTL::JSONObject> jsonObject(
    FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
    );

  std::vector<FTL::CStrRef> execPaths;
  std::vector<std::string> prefixes;
  std::vector<FTL::JSONObject const *> objects;
  execPaths.push_back(currentExecPath);
  prefixes.push_back("");
  objects.push_back(jsonObject.get());

  for(size_t i=0;i<objects.size();i++)
  {
    FTL::JSONObject const * varsObject = objects[i]->maybeGetObject( FTL_STR("vars") );
    if(varsObject)
    {
      for(FTL::JSONObject::const_iterator it = varsObject->begin();
        it != varsObject->end(); it++
        )
      {
        FTL::CStrRef key = it->first;
        std::string path = prefixes[i];
        if(path.length() > 0)
          path += ".";
        path += key.c_str();
        if(words.contains(path.c_str()))
          continue;
        words.append(path.c_str());
      }
    }

    FTL::JSONObject const * subsObject = objects[i]->maybeGetObject( FTL_STR("subs") );
    if(subsObject)
    {
      for(FTL::JSONObject::const_iterator it = subsObject->begin();
        it != subsObject->end(); it++
        )
      {
        FTL::JSONObject const * subGraph = it->second->maybeCast<FTL::JSONObject>();
        if(subGraph)
        {
          std::string path;
          FTL::CStrRef execPath = execPaths[i];
          FTL::CStrRef graphName = it->first;
          if(execPath.size() > 0)
          {
            std::string graphNameStr(graphName);
            std::string execPathStr(execPath);
            if(graphNameStr == execPathStr || 
              graphNameStr + "." == execPathStr.substr(0, graphNameStr.length() + 1))
            {
              execPath = execPath.substr(graphNameStr.length() + 1).data();
            }
            else
            {
              path = graphName;
              execPath = "";
            }
          }
          else
          {
            path = prefixes[i];
            if(path.length() > 0)
              path += ".";
            path += std::string(it->first);
          }

          execPaths.push_back(execPath);
          prefixes.push_back(path);
          objects.push_back(subGraph);
        }
      }      
    }
  }

  return words;
}

void DFGController::cmdRemoveNodes(
  QStringList nodeNames
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoRemoveNodes(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeNames
    );
}

void DFGController::cmdConnect(
  QString srcPath, 
  QString dstPath
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoConnect(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    srcPath,
    dstPath
    );
}

void DFGController::cmdDisconnect(
  QString srcPath, 
  QString dstPath
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoDisconnect(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    srcPath,
    dstPath
    );
}

QString DFGController::cmdAddInstWithEmptyGraph(
  QString title,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddGraph(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    title,
    pos
    );
}

QString DFGController::cmdAddInstWithEmptyFunc(
  QString title,
  QString initialCode,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddFunc(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    title,
    initialCode,
    pos
    );
}

QString DFGController::cmdAddInstFromPreset(
  QString presetPath,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoInstPreset(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    presetPath,
    pos
    );
}

QString DFGController::cmdAddVar(
  QString desiredNodeName,
  QString dataType,
  QString extDep,
  QPointF pos
  )
{
  if ( !validPresetSplit()
     || desiredNodeName.isEmpty()
     || dataType.isEmpty() )
    return QString();

  return m_cmdHandler->dfgDoAddVar(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredNodeName,
    dataType,
    extDep,
    pos
    );
}

QString DFGController::cmdAddGet(
  QString desiredNodeName,
  QString varPath,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddGet(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredNodeName,
    varPath,
    pos
    );
}

QString DFGController::cmdAddSet(
  QString desiredNodeName,
  QString varPath,
  QPointF pos
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoAddSet(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredNodeName,
    varPath,
    pos
    );
}

QString DFGController::cmdAddPort(
  QString desiredPortName,
  FabricCore::DFGPortType portType,
  QString typeSpec,
  QString portToConnect,
  QString extDep,
  QString metaData
  )
{
  if(!validPresetSplit())
    return QString();

  return m_cmdHandler->dfgDoAddPort(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    desiredPortName,
    portType,
    typeSpec,
    portToConnect,
    extDep,
    metaData
    );
}

QString DFGController::cmdCreatePreset(
  QString nodeName,
  QString presetDirPath,
  QString presetName
  )
{
  return m_cmdHandler->dfgDoCreatePreset(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeName,
    presetDirPath,
    presetName
    );
}

QString DFGController::cmdEditPort(
  QString oldPortName,
  QString desiredNewPortName,
  QString typeSpec,
  QString extDep,
  QString uiMetadata
  )
{
  if(!validPresetSplit())
    return QString();

  return m_cmdHandler->dfgDoEditPort(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    oldPortName,
    desiredNewPortName,
    typeSpec,
    extDep,
    uiMetadata
    );
}

void DFGController::cmdRemovePort(
  QString portName
  )
{
  if(!validPresetSplit())
    return;

  m_cmdHandler->dfgDoRemovePort(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    portName
    );
}

void DFGController::cmdMoveNodes(
  QStringList nodeNames,
  QList<QPointF> newTopLeftPoss
  )
{
  m_cmdHandler->dfgDoMoveNodes(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeNames,
    newTopLeftPoss
    );
}

void DFGController::cmdResizeBackDropNode(
  QString backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize
  )
{
  m_cmdHandler->dfgDoResizeBackDrop(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    backDropNodeName,
    newTopLeftPos,
    newSize
    );
}

QString DFGController::cmdImplodeNodes(
  QStringList nodeNames,
  QString desiredNodeName
  )
{
  if(!validPresetSplit())
    return "";

  return m_cmdHandler->dfgDoImplodeNodes(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeNames,
    desiredNodeName
    );
}

QList<QString> DFGController::cmdExplodeNode(
  QString nodeName
  )
{
  if(!validPresetSplit())
    return QList<QString>();

  return m_cmdHandler->dfgDoExplodeNode(
    getBinding(),
    getExecPath_QS(),
    getExec(),
    nodeName
    );
}

void DFGController::gvcDoMoveNodes(
  std::vector<GraphView::Node *> const &nodes,
  QPointF delta,
  bool allowUndo
  )
{
  if ( allowUndo )
  {
    QStringList nodeNames;
    QList<QPointF> newTopLeftPoss;
    nodeNames.reserve( nodes.size() );
    newTopLeftPoss.reserve( nodes.size() );
    for ( std::vector<GraphView::Node *>::const_iterator it = nodes.begin();
      it != nodes.end(); ++it )
    {
      GraphView::Node *node = *it;
      FTL::CStrRef nodeName = node->name();
      nodeNames.append( QString::fromUtf8( nodeName.data(), nodeName.size() ) );
      newTopLeftPoss.append( node->topLeftGraphPos() + delta );
    }

    cmdMoveNodes( nodeNames, newTopLeftPoss );
  }
  else
  {
    for ( std::vector<GraphView::Node *>::const_iterator it = nodes.begin();
      it != nodes.end(); ++it )
    {
      GraphView::Node *node = *it;
      FTL::CStrRef nodeName = node->name();

      QPointF newPos = node->topLeftGraphPos() + delta;

      std::string newPosJSON;
      {
        FTL::JSONEnc<> enc( newPosJSON );
        FTL::JSONObjectEnc<> objEnc( enc );
        {
          FTL::JSONEnc<> xEnc( objEnc, FTL_STR("x") );
          FTL::JSONFloat64Enc<> xS32Enc( xEnc, newPos.x() );
        }
        {
          FTL::JSONEnc<> yEnc( objEnc, FTL_STR("y") );
          FTL::JSONFloat64Enc<> yS32Enc( yEnc, newPos.y() );
        }
      }

      getExec().setNodeMetadata(
        nodeName.c_str(),
        "uiGraphPos",
        newPosJSON.c_str(),
        false,
        false
        );
    }
  }
}

void DFGController::gvcDoResizeBackDropNode(
  GraphView::BackDropNode *backDropNode,
  QPointF newTopLeftPos,
  QSizeF newSize,
  bool allowUndo
  )
{
  if ( allowUndo )
  {
    cmdResizeBackDropNode(
      QString::fromUtf8( backDropNode->name().c_str() ),
      newTopLeftPos,
      newSize
      );
  }
  else
  {
    {
      std::string newPosJSON;
      {
        FTL::JSONEnc<> enc( newPosJSON );
        FTL::JSONObjectEnc<> objEnc( enc );
        {
          FTL::JSONEnc<> xEnc( objEnc, FTL_STR("x") );
          FTL::JSONFloat64Enc<> xS32Enc( xEnc, newTopLeftPos.x() );
        }
        {
          FTL::JSONEnc<> yEnc( objEnc, FTL_STR("y") );
          FTL::JSONFloat64Enc<> yS32Enc( yEnc, newTopLeftPos.y() );
        }
      }

      getExec().setNodeMetadata(
        backDropNode->name().c_str(),
        "uiGraphPos",
        newPosJSON.c_str(),
        false,
        false
        );
    }

    {
      std::string newSizeJSON;
      {
        FTL::JSONEnc<> enc( newSizeJSON );
        FTL::JSONObjectEnc<> objEnc( enc );
        {
          FTL::JSONEnc<> wEnc( objEnc, FTL_STR("w") );
          FTL::JSONFloat64Enc<> wS32Enc( wEnc, newSize.width() );
        }
        {
          FTL::JSONEnc<> hEnc( objEnc, FTL_STR("h") );
          FTL::JSONFloat64Enc<> hS32Enc( hEnc, newSize.height() );
        }
      }

      getExec().setNodeMetadata(
        backDropNode->name().c_str(),
        "uiGraphSize",
        newSizeJSON.c_str(),
        false,
        false
        );
    }
  }
}

void DFGController::setBlockCompilations( bool blockCompilations )
{
  if ( blockCompilations )
    m_host.blockComps();
  else
  {
    m_host.unblockComps();
    emitArgsChanged();
    emit varsChanged();
  }
}

void DFGController::emitNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName
  )
{
  emit nodeRenamed( m_execPath, oldNodeName, newNodeName );
}

void DFGController::emitNodeRemoved( FTL::CStrRef nodeName )
{
  emit nodeRemoved( m_execPath, nodeName );
}


void DFGController::focusNode( FTL::StrRef nodeName )
{
  if ( m_exec.getType() == FabricCore::DFGExecType_Graph )
  {
    GraphView::Graph *gvGraph = graph();
    gvGraph->clearSelection();
    if ( GraphView::Node *gvNode = gvGraph->node( nodeName ) )
    {
      gvNode->setSelected( true );
      frameSelectedNodes();
    }
  }
}

void DFGController::onNotificationTimer()
{
  if ( m_varsChangedPending )
  {
    m_varsChangedPending = false;
    emit varsChanged();
  }
  if ( m_argsChangedPending )
  {
    m_argsChangedPending = false;
    emit argsChanged();
  }
  if ( m_argValuesChangedPending )
  {
    m_argValuesChangedPending = false;
    emit argValuesChanged();
  }
  if ( m_defaultValuesChangedPending )
  {
    m_defaultValuesChangedPending = false;
    emit defaultValuesChanged();
  }
  if ( m_topoDirtyPending )
  {
    m_topoDirtyPending = false;
    emit topoDirty();

    // [pzion 20160405] FE-6269
    // topoDirty implies dirty
    m_dirtyPending = false;
  }
  if ( m_dirtyPending )
  {
    m_dirtyPending = false;
    emit dirty();
  }
}
