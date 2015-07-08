// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>

#include <FTL/JSONEnc.h>
#include <FTL/Str.h>
#include <FTL/MapCharSingle.h>

#include <FabricUI/GraphView/GraphRelaxer.h>

#include "DFGController.h"
#include "DFGLogWidget.h"
#include "DFGNotificationRouter.h"
#include "DFGUICmdHandler.h"
#include "DFGUICmd_QUndo/DFGRenamePortCommand.h"
#include "DFGUICmd_QUndo/DFGSetCodeCommand.h"
#include "DFGUICmd_QUndo/DFGSetArgCommand.h"
#include "DFGUICmd_QUndo/DFGSetDefaultValueCommand.h"
#include "DFGUICmd_QUndo/DFGSetRefVarPathCommand.h"
#include "DFGUICmd_QUndo/DFGSetNodeCacheRuleCommand.h"
#include "DFGUICmd_QUndo/DFGCopyCommand.h"
#include "DFGUICmd_QUndo/DFGPasteCommand.h"

#include <sstream>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGController::DFGController(
  GraphView::Graph * graph,
  FabricCore::Client client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  FabricCore::DFGHost host,
  FabricCore::DFGBinding binding,
  DFGUICmdHandler *cmdHandler,
  FabricServices::Commands::CommandStack * stack,
  bool overTakeBindingNotifications
  )
  : GraphView::Controller(graph, stack)
  , m_client( client )
  , m_host( host )
  , m_binding( binding )
  , m_manager(manager)
  , m_cmdHandler( cmdHandler )
  , m_router(0)
  , m_logFunc(0)
  , m_overTakeBindingNotifications( overTakeBindingNotifications )
{
  m_router = NULL;
  m_logFunc = NULL;
  m_overTakeBindingNotifications = overTakeBindingNotifications;
  m_presetDictsUpToDate = false;

  QObject::connect(this, SIGNAL(argsChanged()), this, SLOT(checkErrors()));
  QObject::connect(this, SIGNAL(variablesChanged()), this, SLOT(onVariablesChanged()));
}

DFGController::~DFGController()
{
}

void DFGController::setClient( FabricCore::Client const &client )
{
  m_client = client;
}

void DFGController::setHost( FabricCore::DFGHost const &host )
{
  m_host = host;
}

void DFGController::setBinding( FabricCore::DFGBinding const &binding )
{
  m_binding = binding;
  m_presetDictsUpToDate = false;
}

DFGNotificationRouter * DFGController::getRouter()
{
  return m_router;
}

FTL::CStrRef DFGController::getExecPath()
{
  return this->getRouter()->getExecPath();
}

FabricCore::DFGExec &DFGController::getExec()
{
  return this->getRouter()->getExec();
}

void DFGController::setRouter(DFGNotificationRouter * router)
{
  if(m_router && m_overTakeBindingNotifications)
    m_binding.setNotificationCallback(NULL, NULL);

  m_router = router;
  if(m_router)
  {
    m_router->setController(this);

    if(m_overTakeBindingNotifications)
      m_binding.setNotificationCallback(
        &BindingNotificationCallback, this
        );
  }
}

bool DFGController::isViewingRootGraph()
{
  return getExecPath().empty();
}

ASTWrapper::KLASTManager * DFGController::astManager()
{
  return m_manager;
}

bool DFGController::gvcDoRemoveNodes(
  FTL::ArrayRef<GraphView::Node *> nodes
  )
{
  if ( !nodes.empty() )
  {
    std::vector<FTL::StrRef> nodeNames;
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
  std::string desc = FTL_STR("Canvas: Create backdrop '");
  desc += title;
  desc += '\'';

  m_cmdHandler->dfgDoAddBackDrop(
    desc,
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
  std::string desc = FTL_STR("Canvas: Change title to '");
  desc += newTitle;
  desc += FTL_STR("' (node '");
  desc += nodeName;
  desc += FTL_STR("')");

  m_cmdHandler->dfgDoSetNodeTitle(
    desc,
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
  std::string desc = FTL_STR("Canvas: ");
  desc += !comment.empty()? FTL_STR("Change"): FTL_STR("Remove");
  desc += FTL_STR(" comment (node '");
  desc += nodeName;
  desc += FTL_STR("')");

  m_cmdHandler->dfgDoSetNodeComment(
    desc,
    getBinding(),
    getExecPath(),
    getExec(),
    nodeName,
    comment
    );
}

void DFGController::cmdSetNodeCommentExpanded(
  FTL::CStrRef nodeName,
  bool expanded
  )
{
  std::string desc = FTL_STR("Canvas: ");
  desc += expanded? FTL_STR("Expand"): FTL_STR("Contract");
  desc += FTL_STR(" comment (node '");
  desc += nodeName;
  desc += FTL_STR("')");

  m_cmdHandler->dfgDoSetNodeCommentExpanded(
    desc,
    getBinding(),
    getExecPath(),
    getExec(),
    nodeName,
    expanded
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
  cmdSetNodeCommentExpanded( node->name(), expanded );
}

std::string DFGController::renamePortByPath(char const *path, char const *name)
{
  FTL::StrRef pathRef(path);
  if(pathRef == name)
    return "";
  try
  {
    DFGRenamePortCommand * command = new DFGRenamePortCommand(this, path, name);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit argsChanged();
    emit structureChanged();
    emit recompiled();

    std::string newName = command->getResult();
    emit execPortRenamed(path, newName.c_str());
    return newName;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return "";
  }
  return "";
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

bool DFGController::setCode(char const * path, char const * code)
{
  try
  {
    Commands::Command * command = new DFGSetCodeCommand(this, path, code);;
    if(!addCommand(command))
    {
      delete(command);
      return false;
    }
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }

  try
  {
    m_binding.execute();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  emit recompiled();
  return true;
}

std::string DFGController::reloadCode(char const * path)
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
            if(DFGController::setCode(path, klCode.c_str()))
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

bool DFGController::setArg(char const * argName, char const * dataType, char const * json)
{
  beginInteraction();
  try
  {
    Commands::Command * command = new DFGSetArgCommand(this, argName, dataType, json);;
    if(addCommand(command))
    {
      bindUnboundRTVals();
      emit argsChanged();
      endInteraction();
      return true;
    }
    delete(command);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  endInteraction();
  return false;
}

bool DFGController::setArg(char const * argName, FabricCore::RTVal value)
{
  try
  {
    Commands::Command * command = new DFGSetArgCommand(this, argName, value);;
    if(addCommand(command))
    {
      emit argsChanged();
      return true;
    }
    delete(command);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

bool DFGController::setDefaultValue(char const * path, FabricCore::RTVal value)
{
  try
  {
    Commands::Command * command = new DFGSetDefaultValueCommand(this, path, value);;
    if(addCommand(command))
    {
      emit argsChanged();
      return true;
    }
    delete(command);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

bool DFGController::setDefaultValue(char const * path, char const * dataType, char const * json)
{
  try
  {
    FabricCore::RTVal value = FabricCore::ConstructRTValFromJSON(m_client, dataType, json);
    return setDefaultValue(path, value);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

std::string DFGController::exportJSON(char const * path)
{
  try
  {
    return m_binding.exportJSON().getCString();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

bool DFGController::setNodeCacheRule(char const * path, FEC_DFGCacheRule rule)
{
  try
  {
    FabricCore::DFGExec &exec = getExec();
    FEC_DFGCacheRule prevRule = exec.getInstCacheRule(path);
    
    if(prevRule == rule)
      return false;
    
    Commands::Command * command = new DFGSetNodeCacheRuleCommand(this, path, rule);;
    if(addCommand(command))
      return true;
    else
      delete(command);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

bool DFGController::setRefVarPath(char const *  path, char const * varPath)
{
  try
  {
    Commands::Command * command = new DFGSetRefVarPathCommand(this, path, varPath);;
    if(addCommand(command))
      return true;
    else
      delete(command);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

bool DFGController::moveNode(char const * path, QPointF pos, bool isTopLeftPos)
{
  try
  {
    FabricCore::DFGExec &exec = getExec();
    QString metaData = "{\"x\": "+QString::number(pos.x())+", \"y\": "+QString::number(pos.y())+"}";
    exec.setNodeMetadata(path, "uiGraphPos", metaData.toUtf8().constData(), false);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

bool DFGController::moveNode(GraphView::Node * uiNode, QPointF pos, bool isTopLeftPos)
{
  if(!isTopLeftPos)
  {
    pos = uiNode->centralPosToTopLeftPos(pos);
    isTopLeftPos = true;
  }

  if(uiNode->type() == GraphView::QGraphicsItemType_Node)
    return moveNode(uiNode->name().c_str(), pos, isTopLeftPos);

  return false;
}

bool DFGController::zoomCanvas(float zoom)
{
  try
  {
    FabricCore::DFGExec &exec = getExec();
    QString metaData = "{\"value\": "+QString::number(zoom)+"}";
    exec.setMetadata("uiGraphZoom", metaData.toUtf8().constData(), false);
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
    QString metaData = "{\"x\": "+QString::number(pan.x())+", \"y\": "+QString::number(pan.y())+"}";
    exec.setMetadata("uiGraphPan", metaData.toUtf8().constData(), false);
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

  for(unsigned int i=0;i<relaxer.numNodes();i++)
  {
    moveNode(relaxer.getName(i).toUtf8().constData(), relaxer.getPos(i), true);
  }

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

std::string DFGController::copy(QStringList paths)
{
  try
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

    DFGCopyCommand * command = new DFGCopyCommand(this, paths);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }

    return command->getJSON();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  return "";
}

bool DFGController::paste()
{
  try
  {
    DFGPasteCommand * command = new DFGPasteCommand(this);
    if(!addCommand(command))
    {
      delete(command);
      return false;
    }

    emit structureChanged();
    emit recompiled();

    std::vector<std::string> paths = command->getNodePaths();
    if(paths.size() > 0)
    {
      const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();

      for(size_t i=0;i<nodes.size();i++)
        nodes[i]->setSelected(false);

      QRectF bounds;
      for(size_t i=0;i<paths.size();i++)
      {
        GraphView::Node * uiNode = graph()->node(paths[i].c_str());
        if(!uiNode)
          continue;
        bounds = bounds.united(uiNode->boundingRect().translated(uiNode->topLeftGraphPos()));
      }

      QGraphicsView * graphicsView = graph()->scene()->views()[0];
      QPointF cursorPos = graphicsView->mapToScene(graphicsView->mapFromGlobal(QCursor::pos()));
      cursorPos = graph()->itemGroup()->mapFromScene(cursorPos);

      QPointF delta = cursorPos - bounds.center();

      for(size_t i=0;i<paths.size();i++)
      {
        GraphView::Node * uiNode = graph()->node(paths[i].c_str());
        if(!uiNode)
          continue;
        uiNode->setSelected(true);
        moveNode(paths[i].c_str(), uiNode->topLeftGraphPos() + delta, true);
      }
    }
    return true;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  return false;
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

  return execute();
}

void DFGController::checkErrors()
{
  FTL::CStrRef execPath = getExecPath();
  FabricCore::DFGExec &exec = getExec();

  unsigned errorCount = exec.getErrorCount();
  for(unsigned i=0;i<errorCount;i++)
  {
    std::string prefixedError = execPath;
    prefixedError += " : ";
    prefixedError += exec.getError(i);
    logError( prefixedError.c_str() );
  }

  if(exec.getType() == FabricCore::DFGExecType_Graph)
  {
    unsigned nodeCount = exec.getNodeCount();
    for(size_t j=0;j<nodeCount;j++)
    {
      char const *nodeName = exec.getNodeName(j);

      if ( !graph() )
        continue;
      GraphView::Node *uiNode = NULL;
      if ( graph() )
      {
        uiNode = graph()->nodeFromPath( nodeName );
        if ( uiNode )
          uiNode->clearError();
      }

      if ( exec.getNodeType(j) == FabricCore::DFGNodeType_Inst )
      {
        FabricCore::DFGExec instExec = exec.getSubExec( nodeName );

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
    // [pzion 20150701] Upgrade old backdrops scheme
    static bool upgradingBackDrops = false;
    if ( !upgradingBackDrops )
    {
      upgradingBackDrops = true;
      std::string uiBackDrops = exec.getMetadata( "uiBackDrops" );
      if ( !uiBackDrops.empty() )
      {
        std::pair<FTL::StrRef, FTL::CStrRef> split = FTL::CStrRef( uiBackDrops ).split(',');
        while ( !split.first.empty() )
        {
          std::string uiBackDropKey = split.first;
          try
          {
            char const *nodeName = exec.addUser( uiBackDropKey.c_str() );

            FTL::CStrRef uiBackDrop = exec.getMetadata( uiBackDropKey.c_str() );
            if ( !uiBackDrop.empty() )
            {
              FTL::JSONStrWithLoc swl( uiBackDrop );
              FTL::OwnedPtr<FTL::JSONObject> jo(
                FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
                );

              exec.setNodeMetadata(
                nodeName,
                "uiTitle",
                jo->getString( FTL_STR("title") ).c_str(),
                false
                );

              exec.setNodeMetadata(
                nodeName,
                "uiGraphPos",
                jo->getObject( FTL_STR("pos") )->encode().c_str(),
                false
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
              exec.setNodeMetadata(
                nodeName, "uiGraphSize", sizeStr.c_str(), false
                );

              exec.setNodeMetadata(
                nodeName,
                "uiNodeColor",
                jo->getObject( FTL_STR("color") )->encode().c_str(),
                false
                );

              exec.setMetadata( uiBackDropKey.c_str(), "", false );
            }
          }
          catch ( ... )
          {
          }
          split = split.second.split(',');
        }
      }
      upgradingBackDrops = false;
    }

    exec.setMetadata( "uiBackDrops", "", false );
  }
}

void DFGController::log(const char * message)
{
  DFGLogWidget::callback(NULL, message, 0);
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


bool DFGController::execute()
{
  try
  {
    m_binding.execute();
    return true;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

void DFGController::onValueChanged(ValueEditor::ValueItem * item)
{
  beginInteraction();
  try
  {
    std::string portOrPinPath = item->path().toUtf8().constData();

    // let's assume it is pin, if there's still a node name in it
    Commands::Command * command = NULL;
    if(portOrPinPath.find('.') != std::string::npos)
    {
      std::string nodeName = portOrPinPath.substr(0, portOrPinPath.find('.'));
      FabricCore::DFGNodeType nodeType = getExec().getNodeType(nodeName.c_str());
      if(nodeType == FabricCore::DFGNodeType_Inst || 
        nodeType == FabricCore::DFGNodeType_Var ||
        (nodeType == FabricCore::DFGNodeType_Set && portOrPinPath == nodeName + ".value"))
      {
        command = new DFGSetDefaultValueCommand(this, portOrPinPath.c_str(), item->value());
      }
      else if((nodeType == FabricCore::DFGNodeType_Get || 
        nodeType == FabricCore::DFGNodeType_Set) && portOrPinPath == nodeName + ".variable")
      {
        command = new DFGSetRefVarPathCommand(this, nodeName.c_str(), item->value().getStringCString());
      }
    }
    else
    {
      command = new DFGSetArgCommand(this, item->name().toUtf8().constData(), item->value());
    }
    if(command == NULL)
      return;
    if(!addCommand(command))
      delete(command);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  endInteraction();
}

bool DFGController::bindUnboundRTVals(FTL::StrRef dataType)
{
  try
  {
    FabricCore::DFGExec &exec = getExec();

    bool argsHaveChanged = false;

    for(size_t i=0;i<exec.getExecPortCount();i++)
    {
      FTL::StrRef dataTypeToCheck = dataType;
      if(dataTypeToCheck.empty())
        dataTypeToCheck = exec.getExecPortResolvedType(i);
      if(dataTypeToCheck.empty())
        continue;
      else if(exec.getExecPortResolvedType(i) != dataTypeToCheck)
        continue;

      if(dataTypeToCheck == "Integer")
        dataTypeToCheck = "SInt32";
      else if(dataTypeToCheck == "Byte")
        dataTypeToCheck = "UInt8";
      else if(dataTypeToCheck == "Size" || dataTypeToCheck == "Count" || dataTypeToCheck == "Index")
        dataTypeToCheck = "UInt32";
      else if(dataTypeToCheck == "Scalar")
        dataTypeToCheck = "Float32";

      // if there is already a bound value, make sure it has the right type
      FabricCore::RTVal value;
      try
      {
        value = m_binding.getArgValue(exec.getExecPortName(i));
      }
      catch(FabricCore::Exception e)
      {
        continue;
      }
      if(value.isValid())
      {
        if(value.getTypeName().getStringCString() == dataTypeToCheck)
          continue;
      }

      addCommand(
        new DFGSetArgCommand(
          this,
          exec.getExecPortName(i),
          dataTypeToCheck.data()
          )
        );
      argsHaveChanged = true;
    }
    return argsHaveChanged;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
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

bool DFGController::setBackDropNodeSize(
  GraphView::BackDropNode * node,
  QSizeF size
  )
{
  try
  {
    std::string uiNodeColorString;
    {
      FTL::JSONEnc<> enc( uiNodeColorString );
      FTL::JSONObjectEnc<> objEnc( enc );
      {
        FTL::JSONEnc<> wEnc( objEnc, FTL_STR("w") );
        FTL::JSONSInt32Enc<> wS32Enc( wEnc, int(size.width()) );
      }
      {
        FTL::JSONEnc<> hEnc( objEnc, FTL_STR("h") );
        FTL::JSONSInt32Enc<> hS32Enc( hEnc, int(size.height()) );
      }
    }

    FabricCore::DFGExec &exec = getExec();
    exec.setNodeMetadata(
      node->name().c_str(),
      "uiGraphSize",
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
    if(descStr == FTL_STR("argChanged"))
    {
      emit argValueChanged(
        jsonObject->getString( FTL_STR("name") ).c_str()
        );
    }
    // [pzion 20150609] Why doesn't this work?
    //
    // else if ( descStr == FTL_STR("argTypeChanged")
    //   || descStr == FTL_STR("argInserted")
    //   || descStr == FTL_STR("argRemoved") )
    // {
    //   emit argsChanged();
    // }
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

  QStringList variables = getVariableWordsFromBinding(m_binding, getExecPath().c_str());
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

  for(size_t i=0;i<m_presetNameSpaceDictSTL.size();i++)
    m_presetNameSpaceDict.add(m_presetNameSpaceDictSTL[i].c_str(), '.', m_presetNameSpaceDictSTL[i].c_str());
  for(size_t i=0;i<m_presetPathDictSTL.size();i++)
    m_presetPathDict.add(m_presetPathDictSTL[i].c_str(), '.', m_presetPathDictSTL[i].c_str());
}

DFGNotificationRouter * DFGController::createRouter(
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
  )
{
  return new DFGNotificationRouter( binding, execPath, exec );
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
              // this variable is in a different sub
              continue;
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
  FTL::ArrayRef<FTL::StrRef> nodeNames
  )
{
  std::stringstream desc;
  desc << FTL_STR("Canvas: Remove ");
  if ( nodeNames.size() > 1 )
  {
    desc << nodeNames.size();
    desc << FTL_STR(" nodes");
  }
  else desc << FTL_STR("node '") << nodeNames[0] << '\'';

  m_cmdHandler->dfgDoRemoveNodes(
    desc.str(),
    getBinding(),
    getExecPath(),
    getExec(),
    nodeNames
    );
}

void DFGController::cmdConnect(
  FTL::StrRef srcPath, 
  FTL::StrRef dstPath
  )
{
  std::string desc = FTL_STR("Canvas: Connect '");
  desc += srcPath;
  desc += FTL_STR("' to '");
  desc += dstPath;
  desc += '\'';

  m_cmdHandler->dfgDoConnect(
    desc,
    getBinding(),
    getExecPath(),
    getExec(),
    srcPath,
    dstPath
    );
}

void DFGController::cmdDisconnect(
  FTL::StrRef srcPath, 
  FTL::StrRef dstPath
  )
{
  std::string desc = FTL_STR("Canvas: Disconnect '");
  desc += srcPath;
  desc += FTL_STR("' from '");
  desc += dstPath;
  desc += '\'';

  m_cmdHandler->dfgDoDisconnect(
    desc,
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
  return m_cmdHandler->dfgDoAddInstWithEmptyGraph(
    FTL_STR("Canvas: Create New Subgraph"),
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
  return m_cmdHandler->dfgDoAddInstWithEmptyFunc(
    FTL_STR("Canvas: Create new function"),
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
  std::string desc = FTL_STR("Canvas: Create instance of preset '");
  desc += presetPath;
  desc += '\'';

  return m_cmdHandler->dfgDoAddInstFromPreset(
    desc,
    getBinding(),
    getExecPath(),
    getExec(),
    presetPath,
    pos
    );
}

std::string DFGController::cmdAddVar(
  FTL::CStrRef desiredNodeName,
  FTL::StrRef dataType,
  FTL::StrRef extDep,
  QPointF pos
  )
{
  return m_cmdHandler->dfgDoAddVar(
    FTL_STR("Canvas: Create new variable node"),
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
  FTL::StrRef varPath,
  QPointF pos
  )
{
  return m_cmdHandler->dfgDoAddGet(
    FTL_STR("Canvas: Create get variable node"),
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
  FTL::StrRef varPath,
  QPointF pos
  )
{
  return m_cmdHandler->dfgDoAddSet(
    FTL_STR("Canvas: Create set variable node"),
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
  std::string desc = FTL_STR("Canvas: Create port '");
  desc += desiredPortName;
  desc += '\'';
  if ( !portToConnect.empty() )
  {
    desc += FTL_STR(" and connect with '");
    desc += portToConnect;
    desc += '\'';
  }

  return m_cmdHandler->dfgDoAddPort(
    desc,
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
  std::string desc = FTL_STR("Canvas: Delete port '");
  desc += portName;
  desc += '\'';

  m_cmdHandler->dfgDoRemovePort(
    desc,
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
  std::stringstream desc;
  desc << FTL_STR("Canvas: Move ");
  desc << nodeNames.size();
  desc << FTL_STR(" node");
  if ( nodeNames.size() != 1 )
    desc << 's';

  m_cmdHandler->dfgDoMoveNodes(
    desc.str(),
    getBinding(),
    getExecPath(),
    getExec(),
    nodeNames,
    newTopLeftPoss
    );
}

std::string DFGController::cmdImplodeNodes(
  FTL::CStrRef desiredNodeName,
  FTL::ArrayRef<FTL::CStrRef> nodeNames
  )
{
  std::stringstream desc;
  desc << FTL_STR("Canvas: Implode ");
  desc << nodeNames.size();
  desc << FTL_STR(" node");
  if ( nodeNames.size() != 1 )
    desc << 's';

  return m_cmdHandler->dfgDoImplodeNodes(
    desc.str(),
    getBinding(),
    getExecPath(),
    getExec(),
    desiredNodeName,
    nodeNames
    );
}

std::vector<std::string> DFGController::cmdExplodeNode(
  FTL::CStrRef nodeName
  )
{
  std::string desc = FTL_STR("Canvas: Explode node '");
  desc += nodeName;
  desc += '\'';

  return m_cmdHandler->dfgDoExplodeNode(
    desc,
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
