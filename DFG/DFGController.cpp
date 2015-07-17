// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>

#include <FTL/JSONEnc.h>
#include <FTL/Str.h>
#include <FTL/MapCharSingle.h>

#include "DFGController.h"
#include "DFGLogWidget.h"
#include "DFGNotificationRouter.h"
#include <FabricUI/GraphView/GraphRelaxer.h>
#include "Commands/DFGAddBackDropCommand.h"
#include "Commands/DFGAddNodeCommand.h"
#include "Commands/DFGAddVarCommand.h"
#include "Commands/DFGAddGetCommand.h"
#include "Commands/DFGAddSetCommand.h"
#include "Commands/DFGAddEmptyGraphCommand.h"
#include "Commands/DFGAddEmptyFuncCommand.h"
#include "Commands/DFGRemoveNodeCommand.h"
#include "Commands/DFGRenameNodeCommand.h"
#include "Commands/DFGAddConnectionCommand.h"
#include "Commands/DFGRemoveConnectionCommand.h"
#include "Commands/DFGRemoveAllConnectionsCommand.h"
#include "Commands/DFGAddPortCommand.h"
#include "Commands/DFGRemovePortCommand.h"
#include "Commands/DFGRenamePortCommand.h"
#include "Commands/DFGSetCodeCommand.h"
#include "Commands/DFGSetArgCommand.h"
#include "Commands/DFGSetDefaultValueCommand.h"
#include "Commands/DFGSetRefVarPathCommand.h"
#include "Commands/DFGSetNodeCacheRuleCommand.h"
#include "Commands/DFGCopyCommand.h"
#include "Commands/DFGPasteCommand.h"
#include "Commands/DFGImplodeNodesCommand.h"
#include "Commands/DFGExplodeNodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGController::DFGController(
  GraphView::Graph * graph,
  DFGWidget *dfgWidget,
  FabricCore::Client &client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  FabricServices::Commands::CommandStack * stack,
  bool overTakeBindingNotifications
  )
  : GraphView::Controller(graph, stack)
  , m_dfgWidget( dfgWidget )
  , m_coreClient(client)
  , m_manager(manager)
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

void DFGController::setClient( FabricCore::Client const &coreClient )
{
  m_coreClient = coreClient;
}

void DFGController::setHostBindingExec(
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
  )
{
  m_coreDFGHost = host;

  setBindingExec( binding, execPath, exec );

  emit hostChanged();
}

void DFGController::setBindingExec(
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
  )
{
  m_coreDFGBinding = binding;

  setExec( execPath, exec );

  emit bindingChanged();
}

void DFGController::setExec(
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
  )
{
  m_coreDFGExecPath = execPath;
  m_coreDFGExec = exec;

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
      m_coreDFGBinding.setNotificationCallback( NULL, NULL );
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
      m_coreDFGBinding.setNotificationCallback(
        &BindingNotificationCallback, this
        );
  }
}

std::string DFGController::addDFGNodeFromPreset(
  FTL::StrRef preset,
  QPointF pos
  )
{
  try
  {
    DFGAddNodeCommand * command = new DFGAddNodeCommand(this, preset, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit structureChanged();
    emit recompiled();
    return command->getInstPath();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

std::string  DFGController::addDFGVar(FTL::StrRef varName, FTL::StrRef dataType, FTL::StrRef extension, QPointF pos)
{
  try
  {
    DFGAddVarCommand * command = new DFGAddVarCommand(this, varName, dataType, extension, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit structureChanged();
    emit recompiled();
    emit variablesChanged();
    return command->getNodePath();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

std::string  DFGController::addDFGGet(FTL::StrRef varName, FTL::StrRef varPath, QPointF pos)
{
  try
  {
    DFGAddGetCommand * command = new DFGAddGetCommand(this, varName, varPath, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit structureChanged();
    emit recompiled();
    return command->getNodePath();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

std::string  DFGController::addDFGSet(FTL::StrRef varName, FTL::StrRef varPath, QPointF pos)
{
  try
  {
    DFGAddSetCommand * command = new DFGAddSetCommand(this, varName, varPath, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit structureChanged();
    emit recompiled();
    return command->getNodePath();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

std::string DFGController::addEmptyGraph(char const * title, QPointF pos)
{
  try
  {
    DFGAddEmptyGraphCommand * command =
      new DFGAddEmptyGraphCommand(this, title, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit structureChanged();
    emit recompiled();
    return command->getInstPath();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

std::string DFGController::addEmptyFunc(char const * title, QPointF pos)
{
  try
  {
    DFGAddEmptyFuncCommand * command =
      new DFGAddEmptyFuncCommand(this, title, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }

    std::string instPath = command->getInstPath();

    std::string code;
    code += "dfgEntry {\n";
    code += "  // result = a + b;\n";
    code += "}\n";

    DFGSetCodeCommand * setCodeCommand = new DFGSetCodeCommand(this, instPath.c_str(), code.c_str());
    if(!addCommand(setCodeCommand))
    {
      delete(setCodeCommand);
      return instPath.c_str();
    }

    emit structureChanged();
    emit recompiled();
    return instPath.c_str();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  return "";
}

void DFGController::addBackDropNode( FTL::CStrRef title, QPointF pos )
{
  try
  {
    DFGAddBackDropCommand *addBackDropCommand =
      new DFGAddBackDropCommand( this, title, pos );
    if ( !addCommand( addBackDropCommand ) )
    {
      delete addBackDropCommand;
      return;
    }
    emit structureChanged();
  }
  catch ( FabricCore::Exception e )
  {
    logError(e.getDesc_cstr());
  }
}

bool DFGController::removeNode(char const * path)
{
  try
  {
    // need to do this since the path is going out of 
    // scope throughout this function
    QString pathCopy(path);
    
    bool emitVarChanged = false;
    if(getCoreDFGExec().getNodeType(path) == FabricCore::DFGNodeType_Var)
      emitVarChanged = true;

    DFGRemoveNodeCommand * removeCommand = new DFGRemoveNodeCommand(this, path);
    if(!addCommand(removeCommand))
    {
      delete(removeCommand);
      return false;
    }
    emit structureChanged();
    emit recompiled();
    if(emitVarChanged)
      emit variablesChanged();

    emit nodeDeleted(pathCopy);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

bool DFGController::removeNode(GraphView::Node * node)
{
  if(node->type() == GraphView::QGraphicsItemType_Node)
    return removeNode(node->name().c_str());
  return false;
}

bool DFGController::renameNodeByPath(char const *path, char const *title)
{
  try
  {
    DFGRenameNodeCommand * command = new DFGRenameNodeCommand(this, path, title);
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
  return true;
}

bool DFGController::renameNode(GraphView::Node * node, FTL::StrRef title)
{
  if(node->title() == title)
    return false;
  return renameNodeByPath(node->name().c_str(), title.data());
}

GraphView::Pin * DFGController::addPin(GraphView::Node * node, FTL::StrRef name, GraphView::PortType pType, QColor color, FTL::StrRef dataType)
{
  // disabled, pins are created by the DFGNotificationRouter
  return NULL;
}

bool DFGController::removePin(GraphView::Pin * pin)
{
  // disabled, pins are created by the DFGNotificationRouter
  return false;
}

std::string DFGController::addPortByPath(
  FTL::StrRef execPath,
  FTL::StrRef name,
  FabricCore::DFGPortType pType,
  FTL::StrRef dataType,
  bool setArgValue
  )
{
  GraphView::PortType portType = GraphView::PortType_Input;
  if(pType == FabricCore::DFGPortType_In)
    portType = GraphView::PortType_Output;
  else if(pType == FabricCore::DFGPortType_IO)
    portType = GraphView::PortType_IO;
  return addPortByPath(execPath, name, portType, dataType, setArgValue);
}

std::string DFGController::addPortByPath(
  FTL::StrRef execPath,
  FTL::StrRef name,
  GraphView::PortType pType,
  FTL::StrRef dataType,
  bool setArgValue
  )
{
  std::string result;
  try
  {
    DFGAddPortCommand * command =
      new DFGAddPortCommand(this, execPath, name, pType, dataType);
    if(!addCommand(command))
      delete(command);
    else
      result = command->getPortPath();

    // if this port is on the binding graph
    if(isViewingRootGraph() && setArgValue)
    {
      if(!dataType.empty() && dataType[0] != '$')
      {
        DFGSetArgCommand * argCommand =
          new DFGSetArgCommand(this, command->getPortPath(), dataType);
        if(!addCommand(argCommand))
          delete(argCommand);
      }        
    }

    emit structureChanged();
    emit recompiled();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return result;
}

bool DFGController::removePortByName(char const *name)
{
  try
  {
    DFGRemovePortCommand * command = new DFGRemovePortCommand(this, name);
    if(!addCommand(command))
      delete(command);
    emit argsChanged();
    emit structureChanged();
    emit recompiled();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

GraphView::Port * DFGController::addPortFromPin(GraphView::Pin * pin, GraphView::PortType pType)
{
  if(!graph())
    return NULL;

  try
  {
    beginInteraction();
    std::string portPath =
      addPortByPath(
        m_coreDFGExecPath,
        pin->name(),
        pType,
        pin->dataType()
        );

    // copy the default value into the port
    if(portPath.length() > 0)
    {
      if(m_coreDFGExec.getNodePortType(pin->path().c_str()) == FabricCore::DFGPortType_In)
      {
        // if this is the graph on the binding, we need to set the arg value
        FTL::CStrRef resolvedType =
          m_coreDFGExec.getNodePortResolvedType(pin->path().c_str());
        if(!resolvedType.empty())
        {
          if(resolvedType == "Integer")
            resolvedType = "SInt32";
          else if(resolvedType == "Byte")
            resolvedType = "UInt8";
          else if(resolvedType == "Size"
            || resolvedType == "Count"
            || resolvedType == "Index")
            resolvedType = "UInt32";
          else if(resolvedType == "Scalar")
            resolvedType = "Float32";

          FabricCore::RTVal defaultValue =
          m_coreDFGExec.getInstPortResolvedDefaultValue(
            pin->path().c_str(),
            resolvedType.c_str()
            );
          if(defaultValue.isValid())
          {
            if(isViewingRootGraph())
              setArg(portPath.c_str(), defaultValue);
            else
              setDefaultValue(portPath.c_str(), defaultValue);
          }
        }

        if(m_coreDFGExec.getNodeType(pin->node()->name().c_str()) == FabricCore::DFGNodeType_Inst)
        {
          FabricCore::DFGExec subExec = m_coreDFGExec.getSubExec(pin->node()->name().c_str());

          // copy all of the metadata
          const char * uiRange =
            subExec.getExecPortMetadata(pin->name().c_str(), "uiRange");
          if(uiRange)
          {
            if(strlen(uiRange) > 0)
            {
              m_coreDFGExec.setExecPortMetadata(portPath.c_str(), "uiRange", uiRange, false);
            }
          }
          const char * uiCombo =
            subExec.getExecPortMetadata(pin->name().c_str(), "uiCombo");
          if(uiCombo)
          {
            if(strlen(uiCombo) > 0)
            {
              m_coreDFGExec.setExecPortMetadata(portPath.c_str(), "uiCombo", uiCombo, false);
            }
          }
          const char * uiHidden =
            subExec.getExecPortMetadata(pin->name().c_str(), "uiHidden");
          if(uiHidden)
          {
            if(strlen(uiHidden) > 0)
            {
              m_coreDFGExec.setExecPortMetadata(portPath.c_str(), "uiHidden", uiHidden, false);
            }
          }
        }

      }
    }

    if(portPath.length() > 0)
    {
      std::vector<GraphView::Connection*> connections = graph()->connections();
      if(pType == GraphView::PortType_Output)
      {
        for(size_t i=0;i<connections.size();i++)
        {
          if(connections[i]->dst() == pin)
          {
            if(!removeConnection(connections[i]->src(), connections[i]->dst()))
            {
              endInteraction();
              return NULL;
            }
            break;
          }
        }
        addConnection(portPath.c_str(), pin->path().c_str());
      }
      else if(pType == GraphView::PortType_Input)
      {
        for(size_t i=0;i<connections.size();i++)
        {
          if(connections[i]->dst()->targetType() == GraphView::TargetType_Port)
          {
            if(((GraphView::Port*)connections[i]->dst())->path() == portPath)
            {
              if(!removeConnection(connections[i]->src(), connections[i]->dst()))
              {
                endInteraction();
                return NULL;
              }
              break;
            }
          }
        }
        addConnection(pin->path().c_str(), portPath.c_str());
      }
    }
    endInteraction();
    return graph()->port(portPath.c_str());
  }
  catch(FabricCore::Exception e)
  {
    endInteraction();
    logError(e.getDesc_cstr());
  }
  return NULL;
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

bool DFGController::addConnection(char const * srcPath, char const * dstPath)
{
  beginInteraction();
  try
  {
    Commands::Command * command = new DFGAddConnectionCommand(this, 
      srcPath, 
      dstPath
    );
    if(addCommand(command))
    {
      bindUnboundRTVals();
      endInteraction();
      emit argsChanged();
      emit structureChanged();
      emit recompiled();
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

bool DFGController::addConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst)
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
  return addConnection(srcPath.c_str(), dstPath.c_str());
}

bool DFGController::removeConnectionByPath(char const *srcPath, char const *dstPath)
{
  try
  {
    Commands::Command * command = new DFGRemoveConnectionCommand(this, 
      srcPath, 
      dstPath
    );

    if(addCommand(command))
    {
      emit argsChanged();
      emit structureChanged();
      emit recompiled();
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

bool DFGController::removeConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst)
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
  return removeConnectionByPath(srcPath.c_str(), dstPath.c_str());
}

bool DFGController::removeAllConnections(const char * path)
{
  try
  {
    Commands::Command * command = new DFGRemoveAllConnectionsCommand(this, path);

    if(addCommand(command))
    {
      emit argsChanged();
      emit structureChanged();
      emit recompiled();
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

bool DFGController::addExtensionDependency(char const * extension, char const * execPath, std::string & errorMessage)
{
  try
  {
    m_coreClient.loadExtension(extension, "", false);

    FabricCore::DFGBinding binding = getCoreDFGBinding();
    FabricCore::DFGExec exec = binding.getExec();
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

bool DFGController::setCode( FTL::CStrRef code )
{
  try
  {
    Commands::Command * command =
      new DFGSetCodeCommand(
        this,
        m_coreDFGExecPath.c_str(),
        code.c_str()
        );
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
    m_coreDFGBinding.execute();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  emit recompiled();
  return true;
}

std::string DFGController::reloadCode()
{
  FabricCore::DFGExec func = getCoreDFGExec();
  if(func.getType() != FabricCore::DFGExecType_Func)
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
            if ( DFGController::setCode( klCode.c_str() ) )
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
    FabricCore::RTVal value = FabricCore::ConstructRTValFromJSON(m_coreClient, dataType, json);
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
    return m_coreDFGBinding.exportJSON().getCString();
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
    FabricCore::DFGExec exec = getCoreDFGExec();
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
    FabricCore::DFGExec exec = getCoreDFGExec();
    
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
    FabricCore::DFGExec exec = getCoreDFGExec();
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
    FabricCore::DFGExec exec = getCoreDFGExec();
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
    FabricCore::DFGExec exec = getCoreDFGExec();
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

    FabricCore::DFGExec exec = getCoreDFGExec();
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

bool DFGController::setNodeComment(GraphView::Node * node, char const * comment)
{
  try
  {
    FabricCore::DFGExec exec = getCoreDFGExec();
    exec.setNodeMetadata(node->name().c_str(), "uiComment", comment, false);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

void DFGController::setNodeCommentExpanded(GraphView::Node * node, bool expanded)
{
  try
  {
    FabricCore::DFGExec exec = getCoreDFGExec();
    exec.setNodeMetadata(node->name().c_str(), "uiCommentExpanded", expanded ? "true" : NULL, false);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
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

std::string DFGController::implodeNodes(char const * desiredName, QStringList paths)
{
  beginInteraction();
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

    QRectF bounds;
    for(int i=0;i<paths.size();i++)
    {
      GraphView::Node * uiNode = graph()->node(paths[i].toUtf8().constData());
      if(uiNode)
        bounds = bounds.united(uiNode->boundingRect().translated(uiNode->topLeftGraphPos()));
    }

    DFGImplodeNodesCommand * command = new DFGImplodeNodesCommand(this, desiredName, paths);
    if(!addCommand(command))
    {
      delete(command);
      endInteraction();
      return "";
    }

    for(int i=0;i<paths.size();i++)
    {
      emit nodeDeleted(paths[i]);
    }

    char const * nodePath = command->getInstName();
    moveNode(nodePath, bounds.center(), false);

    GraphView::Node * uiNode = graph()->node(nodePath);
    if(uiNode)
      uiNode->setSelected(true);

    endInteraction();
    return nodePath;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  endInteraction();
  return "";
}

QStringList DFGController::explodeNode(char const * path)
{
  QStringList result;

  beginInteraction();
  try
  {
    FTL::StrRef pathRef(path);
    if(pathRef.size() == 0)
    {
      const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
      for(unsigned int i=0;i<nodes.size();i++)
      {
        if(nodes[i]->type() != GraphView::QGraphicsItemType_Node)
          continue;
        path = nodes[i]->name().c_str();
        break;
      }
    }

    QRectF oldBound;
    GraphView::Node * uiNode = graph()->node(path);
    if(uiNode)
      oldBound = oldBound.united(uiNode->boundingRect().translated(uiNode->topLeftGraphPos()));

    DFGExplodeNodeCommand * command = new DFGExplodeNodeCommand(this, path);
    if(!addCommand(command))
    {
      delete(command);
      endInteraction();
      return result;
    }

    std::vector<std::string> nodeNames = command->getNodeNames();
    for(size_t i=0;i<nodeNames.size();i++)
      result.append(nodeNames[i].c_str());

    QRectF newBounds;
    for(int i=0;i<result.length();i++)
    {
      GraphView::Node * uiNode = graph()->node(result[i].toUtf8().constData());
      if(uiNode)
      {
        newBounds = newBounds.united(uiNode->boundingRect().translated(uiNode->topLeftGraphPos()));
      }
    }

    QPointF delta = oldBound.center() - newBounds.center();
    for(int i=0;i<result.length();i++)
    {
      GraphView::Node * uiNode = graph()->node(result[i].toUtf8().constData());
      if(uiNode)
      {
        moveNode(result[i].toUtf8().constData(), uiNode->topLeftGraphPos() + delta, true);
        uiNode->setSelected(true);
      }
    }

    endInteraction();
    return result;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  endInteraction();
  return result;
}

bool DFGController::reloadExtensionDependencies(char const * path)
{
  FabricCore::DFGExec exec = getCoreDFGExec();
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
      m_coreClient.loadExtension(ext, version.c_str(), true);
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
  unsigned errorCount = m_coreDFGExec.getErrorCount();
  for(unsigned i=0;i<errorCount;i++)
  {
    std::string prefixedError = m_coreDFGExecPath;
    prefixedError += " : ";
    prefixedError += m_coreDFGExec.getError(i);
    logError( prefixedError.c_str() );
  }

  if(m_coreDFGExec.getType() == FabricCore::DFGExecType_Graph)
  {
    unsigned nodeCount = m_coreDFGExec.getNodeCount();
    for(size_t j=0;j<nodeCount;j++)
    {
      char const *nodeName = m_coreDFGExec.getNodeName(j);

      if ( !graph() )
        continue;
      GraphView::Node *uiNode = NULL;
      if ( graph() )
      {
        uiNode = graph()->nodeFromPath( nodeName );
        if ( uiNode )
          uiNode->clearError();
      }

      if ( m_coreDFGExec.getNodeType(j) == FabricCore::DFGNodeType_Inst )
      {
        FabricCore::DFGExec instExec = m_coreDFGExec.getSubExec( nodeName );

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
    
    char const * uiBackDropsCStr = m_coreDFGExec.getMetadata( "uiBackDrops" );
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
          char const *nodeName = m_coreDFGExec.addUser( uiBackDropKey.c_str() );

          FTL::CStrRef uiBackDrop = m_coreDFGExec.getMetadata( uiBackDropKey.c_str() );
          if ( !uiBackDrop.empty() )
          {
            FTL::JSONStrWithLoc swl( uiBackDrop );
            FTL::OwnedPtr<FTL::JSONObject> jo(
              FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
              );

            m_coreDFGExec.setNodeMetadata(
              nodeName,
              "uiTitle",
              jo->getString( FTL_STR("title") ).c_str(),
              false
              );

            FTL::JSONObject const *posJO =
              jo->getObject( FTL_STR("pos") );
            std::string posStr;
            {
              FTL::JSONEnc<> posEnc( posStr );
              posJO->encode( posEnc );
            }
            m_coreDFGExec.setNodeMetadata(
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
            m_coreDFGExec.setNodeMetadata(
              nodeName, "uiGraphSize", sizeStr.c_str(), false
              );

            FTL::JSONObject const *colorJO =
              jo->getObject( FTL_STR("color") );
            std::string colorStr;
            {
              FTL::JSONEnc<> colorEnc( colorStr );
              colorJO->encode( colorEnc );
            }
            m_coreDFGExec.setNodeMetadata(
              nodeName, "uiNodeColor", colorStr.c_str(), false
              );

            m_coreDFGExec.setMetadata( uiBackDropKey.c_str(), "", false );
          }
        }
        catch ( ... )
        {
        }
        split = split.second.split(',');
      }
  
      m_coreDFGExec.setMetadata( "uiBackDrops", "", false );
    }
    upgradingBackDrops = false;
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
    m_coreDFGBinding.execute();
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

    FabricCore::DFGExec rootExec = m_coreDFGBinding.getExec();

    // let's assume it is pin, if there's still a node name in it
    Commands::Command * command = NULL;
    if(portOrPinPath.rfind('.') != std::string::npos)
    {
      std::string nodePath = portOrPinPath.substr(0, portOrPinPath.rfind('.'));
      std::string nodeName = nodePath;
      FabricCore::DFGExec parentExec = rootExec;

      // if we have a path deeper than one level, 
      // determine the sub exec
      size_t pos = nodePath.rfind('.');
      if(pos != std::string::npos)
      {
        parentExec = parentExec.getSubExec(nodePath.substr(0, pos).c_str());
        nodeName = nodePath.substr(pos+1);
      }

      FabricCore::DFGNodeType nodeType = parentExec.getNodeType(nodeName.c_str());
      if(nodeType == FabricCore::DFGNodeType_Inst || 
        nodeType == FabricCore::DFGNodeType_Var ||
        (nodeType == FabricCore::DFGNodeType_Set && portOrPinPath == nodePath + ".value"))
      {
        command = new DFGSetDefaultValueCommand(this, portOrPinPath.c_str(), item->value());
      }
      else if((nodeType == FabricCore::DFGNodeType_Get || 
        nodeType == FabricCore::DFGNodeType_Set) && portOrPinPath == nodePath + ".variable")
      {
        command = new DFGSetRefVarPathCommand(this, nodePath.c_str(), item->value().getStringCString());
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
    FabricCore::DFGExec rootExec = m_coreDFGBinding.getExec();

    bool argsHaveChanged = false;

    for(size_t i=0;i<rootExec.getExecPortCount();i++)
    {
      FTL::StrRef dataTypeToCheck = dataType;
      if(dataTypeToCheck.empty())
        dataTypeToCheck = rootExec.getExecPortResolvedType(i);
      if(dataTypeToCheck.empty())
        continue;
      else if(rootExec.getExecPortResolvedType(i) != dataTypeToCheck)
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
        value = m_coreDFGBinding.getArgValue(rootExec.getExecPortName(i));
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
          rootExec.getExecPortName(i),
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
    FabricCore::DFGStringResult result = getCoreDFGExec().canConnectTo( pathA, pathB );
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
      FabricCore::DFGExec exec = getCoreDFGExec();
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

    FabricCore::DFGExec exec = getCoreDFGExec();
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
    FabricCore::DFGExec exec = getCoreDFGExec();
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
  if(m_presetDictsUpToDate || !m_coreDFGHost.isValid())
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
      m_coreDFGBinding,
      m_coreDFGExecPath.c_str()
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
      FabricCore::DFGStringResult jsonStr = m_coreDFGHost.getPresetDesc(paths[i].c_str());
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

DFGNotificationRouter *DFGController::createRouter()
{
  DFGNotificationRouter *result = new DFGNotificationRouter( this );
  QObject::connect(
    this, SIGNAL(execChanged()),
    result, SLOT(onExecChanged())
    );
  return result;
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

void DFGController::setBlockCompilations( bool blockCompilations )
{
  if ( blockCompilations )
    m_coreDFGHost.blockComps();
  else
  {
    m_coreDFGHost.unblockComps();
    emit structureChanged();
    emit recompiled();
    emit variablesChanged();
  }
}
