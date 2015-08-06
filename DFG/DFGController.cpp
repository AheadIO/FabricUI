// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>

#include <FTL/JSONEnc.h>
#include <FTL/Str.h>
#include <FTL/MapCharSingle.h>

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphRelaxer.h>

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/DFGNotificationRouter.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGUIUtil.h>

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
  , m_dirtyPending( false )
{
  m_router = NULL;
  m_logFunc = NULL;
  m_overTakeBindingNotifications = overTakeBindingNotifications;
  m_presetDictsUpToDate = false;

  QObject::connect(this, SIGNAL(argsChanged()), this, SLOT(checkErrors()));
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
  m_binding = binding;

  setExec( execPath, exec );

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

DFGNotificationRouter * DFGController::getRouter()
{
  return m_router;
}

void DFGController::setRouter(DFGNotificationRouter * router)
{
  if ( m_router )
  {
    if ( m_overTakeBindingNotifications )
      m_binding.setNotificationCallback( NULL, NULL );
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

    if ( m_overTakeBindingNotifications )
      m_binding.setNotificationCallback(
        &BindingNotificationCallback, this
        );
  }
}

bool DFGController::gvcDoRemoveNodes(
  FTL::ArrayRef<GraphView::Node *> nodes
  )
{
  if ( !nodes.empty() )
  {
    std::vector<FTL::CStrRef> nodeNames;
    nodeNames.reserve( nodes.size() );
    for ( unsigned i = 0; i < nodes.size(); ++i )
      nodeNames.push_back( nodes[i]->name() );

    cmdRemoveNodes( nodeNames );
  }
  return true;
}

void DFGController::cmdAddBackDrop(
  FTL::CStrRef title,
  QPointF pos
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoAddBackDrop(
    getBinding(),
    getExecPath(),
    getExec(),
    title,
    pos
    );
}

void DFGController::cmdSetNodeTitle(
  FTL::CStrRef nodeName,
  FTL::CStrRef newTitle
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoSetNodeTitle(
    getBinding(),
    getExecPath(),
    getExec(),
    nodeName,
    newTitle
    );
}

void DFGController::cmdSetNodeComment(
  FTL::CStrRef nodeName,
  FTL::CStrRef comment
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoSetNodeComment(
    getBinding(),
    getExecPath(),
    getExec(),
    nodeName,
    comment
    );
}

void DFGController::setNodeCommentExpanded(
  FTL::CStrRef nodeName,
  bool expanded
  )
{
  m_exec.setNodeMetadata(
    nodeName.c_str(),
    "uiCommentExpanded",
    expanded? "true": "",
    false
    );
}

bool DFGController::gvcDoAddInstFromPreset(
  FTL::CStrRef presetPath,
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
  FTL::CStrRef desiredPortName,
  GraphView::PortType portType,
  FTL::CStrRef typeSpec,
  GraphView::ConnectionTarget *connectWith
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

  cmdAddPort(
    desiredPortName,
    dfgPortType,
    typeSpec,
    connectWith? FTL::CStrRef( connectWith->path() ): FTL::CStrRef()
    );
}

void DFGController::gvcDoSetNodeCommentExpanded(
  GraphView::Node *node,
  bool expanded
  )
{
  setNodeCommentExpanded( node->name(), expanded );
}

std::string DFGController::cmdRenameExecPort(
  FTL::CStrRef oldName,
  FTL::CStrRef desiredNewName
  )
{
  UpdateSignalBlocker blocker( this );

  return m_cmdHandler->dfgDoRenamePort(
    getBinding(),
    getExecPath(),
    getExec(),
    oldName,
    desiredNewName
    );
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

  cmdConnect( srcPath, dstPath );
  
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

  cmdDisconnect( srcPath.c_str(), dstPath.c_str() );

  return true;
}

bool DFGController::addExtensionDependency(char const * extension, char const * execPath, std::string & errorMessage)
{
  try
  {
    m_client.loadExtension(extension, "", false);

    FabricCore::DFGExec &exec = getExec();
    FTL::StrRef execPathRef(execPath);
    if(execPathRef.size() > 0)
      exec = exec.getSubExec(execPathRef.data());
    exec.addExtDep(extension);
  }
  catch(FabricCore::Exception e)
  {
    errorMessage = e.getDesc_cstr();
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

void DFGController::cmdSetCode( FTL::CStrRef code )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoSetCode(
    getBinding(),
    getExecPath(),
    getExec(),
    code
    );
}

std::string DFGController::reloadCode()
{
  FabricCore::DFGExec &func = getExec();
  if ( func.getType() != FabricCore::DFGExecType_Func )
    return "";

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
            std::string klCode = klCodeVar->getStringData();
            cmdSetCode( klCode.c_str() );
            return klCode.c_str();
          }
        }
      }
    }
    catch(FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
    }
  }    
  return "";
}

void DFGController::cmdSetArgType(
  FTL::CStrRef argName,
  FTL::CStrRef typeName
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoSetArgType(
    getBinding(),
    argName,
    typeName
    );
}

bool DFGController::zoomCanvas(float zoom)
{
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

    exec.setMetadata("uiGraphZoom", json.c_str(), false);
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

    exec.setMetadata("uiGraphPan", json.c_str(), false);
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
    GraphView::Node * uiNode = graph()->node(paths[i].toUtf8().constData());
    if(!uiNode)
      continue;
    rootNodes.push_back(uiNode);
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

  std::vector<FTL::CStrRef> nodeNames;
  nodeNames.reserve( relaxer.numNodes() );
  std::vector<QPointF> newTopLeftPoss;
  newTopLeftPoss.reserve( relaxer.numNodes() );
  for ( unsigned i=0; i<relaxer.numNodes(); i++ )
  {
    nodeNames.push_back( relaxer.getName(i) );
    newTopLeftPoss.push_back( relaxer.getPos(i) );
  }

  cmdMoveNodes( nodeNames, newTopLeftPoss );

  return true;
}

bool DFGController::tintBackDropNode(
  GraphView::BackDropNode * node,
  QColor color
  )
{
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
      node->name().c_str(),
      "uiNodeColor",
      uiNodeColorString.c_str(),
      false
      );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

std::string DFGController::copy()
{
  std::string json;
  try
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();

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
  try
  {
    const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();

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

    std::vector<FTL::CStrRef> pathCStrRefs;
    pathCStrRefs.reserve( pathStrs.size() );
    for ( size_t i = 0; i < pathStrs.size(); ++i )
      pathCStrRefs.push_back( pathStrs[i] );

    cmdRemoveNodes( pathCStrRefs );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

void DFGController::cmdPaste()
{
  UpdateSignalBlocker blocker( this );
  
  try
  {
    QClipboard *clipboard = QApplication::clipboard();
    QGraphicsView *graphicsView = graph()->scene()->views()[0];
    m_cmdHandler->dfgDoPaste(
      getBinding(),
      getExecPath(),
      getExec(),
      clipboard->text().toUtf8().constData(),
      graphicsView->mapToScene(
        graphicsView->mapFromGlobal(QCursor::pos())
        )
      );
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

bool DFGController::reloadExtensionDependencies(char const * path)
{
  FabricCore::DFGExec &exec = getExec();
  FTL::StrRef pathRef(path);
  if(pathRef.size() > 0)
    exec = exec.getSubExec(path);

  if(exec.isValid())
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

void DFGController::checkErrors()
{
  unsigned errorCount = m_exec.getErrorCount();
  for(unsigned i=0;i<errorCount;i++)
  {
    std::string prefixedError = m_execPath;
    prefixedError += " : ";
    prefixedError += m_exec.getError(i);
    logError( prefixedError.c_str() );
  }

  if(m_exec.getType() == FabricCore::DFGExecType_Graph)
  {
    unsigned nodeCount = m_exec.getNodeCount();
    for(size_t j=0;j<nodeCount;j++)
    {
      char const *nodeName = m_exec.getNodeName(j);

      if ( !graph() )
        continue;
      GraphView::Node *uiNode = NULL;
      if ( graph() )
      {
        uiNode = graph()->nodeFromPath( nodeName );
        if ( uiNode )
          uiNode->clearError();
      }

      if ( m_exec.getNodeType(j) == FabricCore::DFGNodeType_Inst )
      {
        FabricCore::DFGExec instExec = m_exec.getSubExec( nodeName );

        unsigned errorCount = instExec.getErrorCount();
        if ( errorCount > 0 )
        {
          std::string errorComposed;
          errorComposed += nodeName;
          errorComposed += " : ";
          for(size_t i=0;i<errorCount;i++)
          {
            if(i > 0)
              errorComposed += "\n";
            errorComposed += instExec.getError(i);
          }
    
          logError( errorComposed.c_str() );
          if ( uiNode )
            uiNode->setError(errorComposed.c_str());
        }

      }
    }
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
              false
              );

            FTL::JSONObject const *posJO =
              jo->getObject( FTL_STR("pos") );
            std::string posStr = posJO->encode();
            m_exec.setNodeMetadata(
              nodeName, "uiGraphPos", posStr.c_str(), false
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
              nodeName, "uiGraphSize", sizeStr.c_str(), false
              );

            FTL::JSONObject const *colorJO =
              jo->getObject( FTL_STR("color") );
            std::string colorStr = colorJO->encode();
            m_exec.setNodeMetadata(
              nodeName, "uiNodeColor", colorStr.c_str(), false
              );

            m_exec.setMetadata( uiBackDropKey.c_str(), "", false );
          }
        }
        catch ( ... )
        {
        }
        split = split.second.split(',');
      }
  
      m_exec.setMetadata( "uiBackDrops", "", false );
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

void DFGController::onValueItemDelta( ValueEditor::ValueItem *valueItem )
{
  try
  {
    std::string portOrPinPath = valueItem->name();

    FabricCore::DFGExec rootExec = m_binding.getExec();

    // let's assume it is pin, if there's still a node name in it
    size_t pos = portOrPinPath.rfind('.');
    if ( pos != std::string::npos )
    {
      std::string nodePath = portOrPinPath.substr( 0, pos );
      std::string nodeName = nodePath;
      std::string portName = portOrPinPath.substr( pos + 1 );

      FabricCore::DFGExec exec = rootExec;
      std::string execPath;
      
      // if we have a path deeper than one level, 
      // determine the sub exec
      pos = nodePath.rfind('.');
      if ( pos != std::string::npos )
      {
        execPath = nodePath.substr (0, pos );
        exec = rootExec.getSubExec( execPath.c_str() );
        nodeName = nodePath.substr( pos + 1 );
      }

      FabricCore::DFGNodeType nodeType = exec.getNodeType(nodeName.c_str());
      if(nodeType == FabricCore::DFGNodeType_Inst || 
        nodeType == FabricCore::DFGNodeType_Var ||
        (nodeType == FabricCore::DFGNodeType_Set && FTL::CStrRef(portName) == FTL_STR("value")))
      {
        std::string portPath = nodeName + "." + portName;

        cmdSetDefaultValue(
          m_binding,
          execPath,
          exec,
          portPath,
          valueItem->value()
          );
      }
      else if((nodeType == FabricCore::DFGNodeType_Get || 
        nodeType == FabricCore::DFGNodeType_Set) && FTL::CStrRef(portName) == FTL_STR("variable"))
      {
        cmdSetRefVarPath(
          m_binding,
          execPath,
          exec,
          nodePath,
          valueItem->value().getStringCString()
          );
      }
    }
    else
    {
      cmdSetArgValue( portOrPinPath, valueItem->value() );
    }
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

void DFGController::cmdSetDefaultValue(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef portPath,
  FabricCore::RTVal const &value
  )
{
  FabricCore::RTVal currentDefaultValue =
    exec.getPortDefaultValue(
      portPath.c_str(),
      value.getTypeNameCStr()
      );
  if ( !currentDefaultValue
    || !currentDefaultValue.isExEQTo( value ) )
  {
    UpdateSignalBlocker blocker( this );
    
    m_cmdHandler->dfgDoSetPortDefaultValue(
      binding,
      execPath,
      exec,
      portPath,
      value.clone()
      );
  }
}

void DFGController::cmdSetArgValue(
  FTL::CStrRef argName,
  FabricCore::RTVal const &value
  )
{
  FabricCore::RTVal currentValue = m_binding.getArgValue( argName.c_str() );
  if ( !currentValue
    || !currentValue.isExEQTo( value ) )
  {
    UpdateSignalBlocker blocker( this );
  
    m_cmdHandler->dfgDoSetArgValue(
      m_binding,
      argName,
      value.clone()
      );
  }
}

void DFGController::cmdSetRefVarPath(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef refName,
  FTL::CStrRef varPath
  )
{
  FTL::CStrRef currentVarPath = exec.getRefVarPath( refName.c_str() );
  if ( currentVarPath != varPath )
  {
    UpdateSignalBlocker blocker( this );
    
    m_cmdHandler->dfgDoSetRefVarPath(
      binding,
      execPath,
      exec,
      refName,
      varPath
      );
  }
}

void DFGController::onValueItemInteractionEnter( ValueEditor::ValueItem *valueItem )
{
}

void DFGController::onValueItemInteractionDelta( ValueEditor::ValueItem *valueItem )
{
  try
  {
    std::string portOrPinPath = valueItem->name();

    FabricCore::DFGExec rootExec = m_binding.getExec();

    // let's assume it is pin, if there's still a node name in it
    size_t pos = portOrPinPath.rfind('.');
    if ( pos != std::string::npos )
    {
      std::string nodePath = portOrPinPath.substr( 0, pos );
      std::string nodeName = nodePath;
      std::string portName = portOrPinPath.substr( pos + 1 );

      FabricCore::DFGExec exec = rootExec;
      std::string execPath;
      
      // if we have a path deeper than one level, 
      // determine the sub exec
      pos = nodePath.rfind('.');
      if ( pos != std::string::npos )
      {
        execPath = nodePath.substr (0, pos );
        exec = rootExec.getSubExec( execPath.c_str() );
        nodeName = nodePath.substr( pos + 1 );
      }

      FabricCore::DFGNodeType nodeType = exec.getNodeType(nodeName.c_str());
      if(nodeType == FabricCore::DFGNodeType_Inst || 
        nodeType == FabricCore::DFGNodeType_Var ||
        (nodeType == FabricCore::DFGNodeType_Set && FTL::CStrRef(portName) == FTL_STR("value")))
      {
        std::string portPath = nodeName + "." + portName;

        exec.setPortDefaultValue(
          portPath.c_str(),
          valueItem->value().clone(),
          false // canUndo
          );
      }
      else if((nodeType == FabricCore::DFGNodeType_Get || 
        nodeType == FabricCore::DFGNodeType_Set) && FTL::CStrRef(portName) == FTL_STR("variable"))
      {
        assert( false );
      }
    }
    else
    {
      FTL::CStrRef argName = portOrPinPath;
      FabricCore::RTVal const &value = valueItem->value();
      
      m_binding.setArgValue(
        argName.c_str(),
        value.clone(),
        false // canUndo
        );
    }
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

void DFGController::onValueItemInteractionLeave( ValueEditor::ValueItem *valueItem )
{
  UpdateSignalBlocker blocker( this );

  try
  {
    std::string portOrPinPath = valueItem->name();

    FabricCore::DFGExec rootExec = m_binding.getExec();

    // let's assume it is pin, if there's still a node name in it
    size_t pos = portOrPinPath.rfind('.');
    if ( pos != std::string::npos )
    {
      std::string nodePath = portOrPinPath.substr( 0, pos );
      std::string nodeName = nodePath;
      std::string portName = portOrPinPath.substr( pos + 1 );

      FabricCore::DFGExec exec = rootExec;
      std::string execPath;
      
      // if we have a path deeper than one level, 
      // determine the sub exec
      pos = nodePath.rfind('.');
      if ( pos != std::string::npos )
      {
        execPath = nodePath.substr (0, pos );
        exec = rootExec.getSubExec( execPath.c_str() );
        nodeName = nodePath.substr( pos + 1 );
      }

      FabricCore::DFGNodeType nodeType = exec.getNodeType(nodeName.c_str());
      if(nodeType == FabricCore::DFGNodeType_Inst || 
        nodeType == FabricCore::DFGNodeType_Var ||
        (nodeType == FabricCore::DFGNodeType_Set && FTL::CStrRef(portName) == FTL_STR("value")))
      {
        std::string portPath = nodeName + "." + portName;

        FabricCore::RTVal valueAtInteractionEnter =
          valueItem->valueAtInteractionEnter();
        FabricCore::RTVal value = valueItem->value();

        exec.setPortDefaultValue(
          portPath.c_str(),
          valueAtInteractionEnter,
          false // canUndo
          );

        cmdSetDefaultValue(
          m_binding,
          execPath,
          exec,
          portPath,
          value
          );
      }
      else if((nodeType == FabricCore::DFGNodeType_Get || 
        nodeType == FabricCore::DFGNodeType_Set) && FTL::CStrRef(portName) == FTL_STR(".variable"))
      {
        assert( false );
      }
    }
    else
    {
      FTL::CStrRef argName = portOrPinPath;
      FabricCore::RTVal valueAtInteractionEnter =
        valueItem->valueAtInteractionEnter();
      FabricCore::RTVal value = valueItem->value();

      m_binding.setArgValue(
        argName.c_str(),
        valueAtInteractionEnter,
        false // canUndo
        );

      cmdSetArgValue( argName, value );
    }
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
}

bool DFGController::bindUnboundRTVals()
{
  bool argsHaveChanged = false;
  try
  {
    FabricCore::DFGExec rootExec = m_binding.getExec();
    unsigned argCount = rootExec.getExecPortCount();
    for ( unsigned i = 0; i < argCount; ++i )
    {
      FTL::CStrRef dataTypeToCheck = rootExec.getExecPortResolvedType( i );
      if ( dataTypeToCheck.empty() )
        continue;

      // if there is already a bound value, make sure it has the right type
      FabricCore::RTVal value;
      try
      {
        value = m_binding.getArgValue( i );
      }
      catch ( FabricCore::Exception e )
      {
        continue;
      }
      if ( !!value && value.hasType( dataTypeToCheck.c_str() ) )
        continue;

      m_binding.setArgValue(
        i,
        DFGCreateDefaultValue( m_client.getContext(), dataTypeToCheck ),
        false
        );

      argsHaveChanged = true;
    }
  }
  catch ( FabricCore::Exception e )
  {
    logError( e.getDesc_cstr() );
  }
  return argsHaveChanged;
}

bool DFGController::canConnectTo(
  char const *pathA,
  char const *pathB,
  std::string &failureReason
  )
{
  try
  {
    FabricCore::DFGStringResult result = getExec().canConnectTo( pathA, pathB );
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

void DFGController::populateNodeToolbar(GraphView::NodeToolbar * toolbar, GraphView::Node * node)
{
  if(node->type() == GraphView::QGraphicsItemType_Node)
  {
    Controller::populateNodeToolbar(toolbar, node);

    try
    {
      FabricCore::DFGExec &exec = getExec();
      if(exec.getNodeType(node->name().c_str()) == FabricCore::DFGNodeType_Inst)
      {
        toolbar->addTool("node_edit", "node_edit.png");
      }
    }
    catch ( FabricCore::Exception e )
    {
      printf(
        "DFGController::populateNodeToolbar: caught Core exception: %s\n",
        e.getDesc_cstr()
        );
    }
  }
}

void DFGController::nodeToolTriggered(FabricUI::GraphView::Node * node, char const * toolName)
{
  Controller::nodeToolTriggered(node, toolName);

  FTL::StrRef toolNameRef(toolName);
  if(toolNameRef == "node_collapse")
  {
    int collapsedState = (int)node->collapsedState();
    FabricCore::Variant collapsedStateVar = FabricCore::Variant::CreateSInt32(collapsedState);
    FabricCore::DFGExec &exec = getExec();
    exec.setNodeMetadata(node->name().c_str(), "uiCollapsedState", collapsedStateVar.getJSONEncoding().getStringData(), false);
  }
  else if(toolNameRef == "node_edit")
  {
    emit nodeEditRequested(node);
  }
}

void DFGController::onVariablesChanged()
{
  m_presetDictsUpToDate = false;
  updatePresetPathDB();
}

void DFGController::bindingNotificationCallback( FTL::CStrRef jsonStr )
{
  // printf("bindingNotif = %s\n", jsonStr.c_str());

  try
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );

    FTL::CStrRef descStr = jsonObject->getString( FTL_STR("desc") );
    if ( descStr == FTL_STR("dirty") )
    {
      emitDirty();
    }
    else if ( descStr == FTL_STR("argTypeChanged")
      || descStr == FTL_STR("argInserted")
      || descStr == FTL_STR("argRemoved") )
    {
      bindUnboundRTVals();
      emitArgsChanged();
    }
    else if ( descStr == FTL_STR("argChanged") )
    {
      emitArgValuesChanged();
    }
    else if ( descStr == FTL_STR("varInserted")
      || descStr == FTL_STR("varRemoved") )
    {
      emitVarsChanged();
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "DFGController::bindingNotificationCallback: caught Core exception: %s\n",
      e.getDesc_cstr()
      );
  }
  catch ( FTL::JSONException e )
  {
    printf(
      "DFGController::bindingNotificationCallback: caught FTL::JSONException: %s\n",
      e.getDescCStr()
      );
  }
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
  FTL::ArrayRef<FTL::CStrRef> nodeNames
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoRemoveNodes(
    getBinding(),
    getExecPath(),
    getExec(),
    nodeNames
    );
}

void DFGController::cmdConnect(
  FTL::CStrRef srcPath, 
  FTL::CStrRef dstPath
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoConnect(
    getBinding(),
    getExecPath(),
    getExec(),
    srcPath,
    dstPath
    );
}

void DFGController::cmdDisconnect(
  FTL::CStrRef srcPath, 
  FTL::CStrRef dstPath
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoDisconnect(
    getBinding(),
    getExecPath(),
    getExec(),
    srcPath,
    dstPath
    );
}

std::string DFGController::cmdAddInstWithEmptyGraph(
  FTL::CStrRef title,
  QPointF pos
  )
{
  UpdateSignalBlocker blocker( this );
  return m_cmdHandler->dfgDoAddGraph(
    getBinding(),
    getExecPath(),
    getExec(),
    title,
    pos
    );
}

std::string DFGController::cmdAddInstWithEmptyFunc(
  FTL::CStrRef title,
  FTL::CStrRef initialCode,
  QPointF pos
  )
{
  UpdateSignalBlocker blocker( this );
  return m_cmdHandler->dfgDoAddFunc(
    getBinding(),
    getExecPath(),
    getExec(),
    title,
    initialCode,
    pos
    );
}

std::string DFGController::cmdAddInstFromPreset(
  FTL::CStrRef presetPath,
  QPointF pos
  )
{
  UpdateSignalBlocker blocker( this );
  return m_cmdHandler->dfgDoInstPreset(
    getBinding(),
    getExecPath(),
    getExec(),
    presetPath,
    pos
    );
}

std::string DFGController::cmdAddVar(
  FTL::CStrRef desiredNodeName,
  FTL::CStrRef dataType,
  FTL::CStrRef extDep,
  QPointF pos
  )
{
  UpdateSignalBlocker blocker( this );
  return m_cmdHandler->dfgDoAddVar(
    getBinding(),
    getExecPath(),
    getExec(),
    desiredNodeName,
    dataType,
    extDep,
    pos
    );
}

std::string DFGController::cmdAddGet(
  FTL::CStrRef desiredNodeName,
  FTL::CStrRef varPath,
  QPointF pos
  )
{
  UpdateSignalBlocker blocker( this );
  return m_cmdHandler->dfgDoAddGet(
    getBinding(),
    getExecPath(),
    getExec(),
    desiredNodeName,
    varPath,
    pos
    );
}

std::string DFGController::cmdAddSet(
  FTL::CStrRef desiredNodeName,
  FTL::CStrRef varPath,
  QPointF pos
  )
{
  UpdateSignalBlocker blocker( this );
  
  return m_cmdHandler->dfgDoAddSet(
    getBinding(),
    getExecPath(),
    getExec(),
    desiredNodeName,
    varPath,
    pos
    );
}

std::string DFGController::cmdAddPort(
  FTL::CStrRef desiredPortName,
  FabricCore::DFGPortType portType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect
  )
{
  UpdateSignalBlocker blocker( this );
  
  return m_cmdHandler->dfgDoAddPort(
    getBinding(),
    getExecPath(),
    getExec(),
    desiredPortName,
    portType,
    typeSpec,
    portToConnect
    );
}

void DFGController::cmdRemovePort(
  FTL::CStrRef portName
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoRemovePort(
    getBinding(),
    getExecPath(),
    getExec(),
    portName
    );
}

void DFGController::cmdMoveNodes(
  FTL::ArrayRef<FTL::CStrRef> nodeNames,
  FTL::ArrayRef<QPointF> newTopLeftPoss
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoMoveNodes(
    getBinding(),
    getExecPath(),
    getExec(),
    nodeNames,
    newTopLeftPoss
    );
}

void DFGController::cmdResizeBackDropNode(
  FTL::CStrRef backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize
  )
{
  UpdateSignalBlocker blocker( this );
  
  m_cmdHandler->dfgDoResizeBackDrop(
    getBinding(),
    getExecPath(),
    getExec(),
    backDropNodeName,
    newTopLeftPos,
    newSize
    );
}

std::string DFGController::cmdImplodeNodes(
  FTL::ArrayRef<FTL::CStrRef> nodeNames,
  FTL::CStrRef desiredNodeName
  )
{
  UpdateSignalBlocker blocker( this );

  return m_cmdHandler->dfgDoImplodeNodes(
    getBinding(),
    getExecPath(),
    getExec(),
    nodeNames,
    desiredNodeName
    );
}

std::vector<std::string> DFGController::cmdExplodeNode(
  FTL::CStrRef nodeName
  )
{
  UpdateSignalBlocker blocker( this );
  
  return m_cmdHandler->dfgDoExplodeNode(
    getBinding(),
    getExecPath(),
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
    std::vector<FTL::CStrRef> nodeNames;
    std::vector<QPointF> newTopLeftPoss;
    nodeNames.reserve( nodes.size() );
    newTopLeftPoss.reserve( nodes.size() );
    for ( std::vector<GraphView::Node *>::const_iterator it = nodes.begin();
      it != nodes.end(); ++it )
    {
      GraphView::Node *node = *it;
      nodeNames.push_back( node->name() );
      newTopLeftPoss.push_back( node->topLeftGraphPos() + delta );
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
      backDropNode->name(),
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
    emit recompiled();
    emit varsChanged();
  }
}

void DFGController::emitNodeRemoved( FTL::CStrRef nodeName )
{
  std::string nodePathFromRoot = m_execPath;
  if ( !nodePathFromRoot.empty() )
    nodePathFromRoot += '.';
  nodePathFromRoot += nodeName;

  emit nodeRemoved( nodePathFromRoot );
}
