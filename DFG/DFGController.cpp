// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>

#include <FTL/Str.h>
#include <FTL/MapCharSingle.h>

#include "DFGController.h"
#include "DFGLogWidget.h"
#include <GraphView/GraphRelaxer.h>
#include "Commands/DFGAddNodeCommand.h"
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
  FabricCore::Client client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  FabricCore::DFGHost host,
  FabricCore::DFGBinding binding,
  FabricCore::DFGExec exec,
  FabricServices::Commands::CommandStack * stack,
  bool overTakeBindingNotifications)
  : GraphView::Controller(graph, stack)
  , m_coreClient( client )
  , m_coreDFGHost( host )
  , m_coreDFGBinding( binding )
  , m_manager(manager)
{
  m_view = NULL;
  m_logFunc = NULL;
  m_overTakeBindingNotifications = overTakeBindingNotifications;
  m_presetDictsUpToDate = false;

  QObject::connect(this, SIGNAL(argsChanged()), this, SLOT(checkErrors()));
}

void DFGController::setClient( FabricCore::Client const &coreClient )
{
  m_coreClient = coreClient;
}

DFGWrapper::Binding DFGController::getCoreDFGBinding()
{
  return getView()->getCoreDFGGraph()->getBinding();
}

DFGWrapper::GraphExecutablePtr DFGController::getGraphExec()
{
  return getView()->getCoreDFGGraph();
}

void DFGController::setHost( FabricCore::DFGHost const &coreDFGHost )
{
  m_coreDFGHost = coreDFGHost;
}

DFGView * DFGController::getView()
{
  return m_view;
}

void DFGController::setView(DFGView * view)
{
  if(m_view && m_overTakeBindingNotifications)
    getBinding().setNotificationCallback(NULL, NULL);

  m_view = view;
  if(m_view)
  {
    m_view->setController(this);
    try
    {
      m_view->onGraphSet();
    }
    catch(FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
    }

    if(m_overTakeBindingNotifications)
      getBinding().setNotificationCallback(bindingNotificationCallback, this);
  }
}

bool DFGController::isViewingRootGraph()
{
  DFGWrapper::GraphExecutablePtr graphExec = getGraphExec();
  if(!graphExec)
    return false;
  return std::string(graphExec->getExecPath()) == "";
}

ASTWrapper::KLASTManager * DFGController::astManager()
{
  return m_manager;
}

QString DFGController::addNodeFromPreset(QString path, QString preset, QPointF pos)
{
  try
  {
    DFGAddNodeCommand * command = new DFGAddNodeCommand(this, path, preset, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit structureChanged();
    emit recompiled();
    return command->getInstPath().c_str();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

QString DFGController::addEmptyGraph(QString path, QString title, QPointF pos)
{
  try
  {
    DFGAddEmptyGraphCommand * command = new DFGAddEmptyGraphCommand(this, path, title, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }
    emit structureChanged();
    emit recompiled();
    return command->getInstPath().c_str();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

QString DFGController::addEmptyFunc(QString path, QString title, QPointF pos)
{
  try
  {
    DFGAddEmptyFuncCommand * command = new DFGAddEmptyFuncCommand(this, path, title, pos);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }

    std::string path = command->getInstPath();

    std::string code;
    code += "dfgEntry {\n";
    code += "  // result = a + b;\n";
    code += "}\n";

    DFGSetCodeCommand * setCodeCommand = new DFGSetCodeCommand(this, path.c_str(), code.c_str());
    if(!addCommand(setCodeCommand))
    {
      delete(setCodeCommand);
      return path.c_str();
    }

    emit structureChanged();
    emit recompiled();
    return path.c_str();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  return "";
}

bool DFGController::removeNode(QString path)
{
  try
  {
    DFGRemoveNodeCommand * removeCommand = new DFGRemoveNodeCommand(this, path);
    if(!addCommand(removeCommand))
    {
      delete(removeCommand);
      return false;
    }
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

bool DFGController::removeNode(GraphView::Node * node)
{
  return removeNode(node->path());
}

bool DFGController::renameNode(QString path, QString title)
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

bool DFGController::renameNode(GraphView::Node * node, QString title)
{
  if(node->title() == title)
    return false;
  return renameNode(node->path(), title);
}

GraphView::Pin * DFGController::addPin(GraphView::Node * node, QString name, GraphView::PortType pType, QColor color, QString dataType)
{
  // disabled, pins are created by the DFGView
  return NULL;
}

bool DFGController::removePin(GraphView::Pin * pin)
{
  // disabled, pins are created by the DFGView
  return false;
}

QString DFGController::addPort(QString path, QString name, FabricCore::DFGPortType pType, QString dataType, bool setArgValue)
{
  GraphView::PortType portType = GraphView::PortType_Input;
  if(pType == FabricCore::DFGPortType_In)
    portType = GraphView::PortType_Output;
  else if(pType == FabricCore::DFGPortType_IO)
    portType = GraphView::PortType_IO;
  return addPort(path, name, portType, dataType, setArgValue);
}

QString DFGController::addPort(QString path, QString name, GraphView::PortType pType, QString dataType, bool setArgValue)
{
  QString result;
  try
  {
    DFGAddPortCommand * command = new DFGAddPortCommand(this, path, name, pType, dataType);
    if(!addCommand(command))
      delete(command);
    else
      result = command->getPortPath();

    // if this port is on the binding graph
    if(isViewingRootGraph() && setArgValue)
    {
      if(dataType[0] != '$')
      {
        DFGSetArgCommand * argCommand = new DFGSetArgCommand(this, command->getPortPath(), dataType);
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

bool DFGController::removePort(QString path, QString name)
{
  try
  {
    DFGRemovePortCommand * command = new DFGRemovePortCommand(this, path, name);
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
    QString portPath = addPort("", pin->name(), pType, pin->dataType());

    // copy the default value into the port
    DFGWrapper::PortPtr port = getPortFromPath(pin->path().toUtf8().constData());
    if(port && portPath.length() > 0)
    {
      if(port->getNodePortType() == FabricCore::DFGPortType_In)
      {
        // if this is the graph on the binding, we need to set the arg value
        DFGWrapper::InstPortPtr instPort = DFGWrapper::InstPortPtr::StaticCast(port);
        std::string resolvedType = instPort->getResolvedType();
        if(resolvedType.length() > 0)
        {
          if(resolvedType == "Integer")
            resolvedType = "SInt32";
          else if(resolvedType == "Byte")
            resolvedType = "UInt8";
          else if(resolvedType == "Size" || resolvedType == "Count" || resolvedType == "Index")
            resolvedType = "UInt32";
          else if(resolvedType == "Scalar")
            resolvedType = "Float32";

          FabricCore::RTVal defaultValue = instPort->getDefaultValue(resolvedType.c_str());
          if(!defaultValue.isValid())
            defaultValue = instPort->getExecPort()->getDefaultValue(resolvedType.c_str());

          if(defaultValue.isValid())
          {
            if(isViewingRootGraph())
              setArg(portPath, defaultValue);
            else
              setDefaultValue(portPath, defaultValue);
          }
        }

        // copy all of the metadata
        port = getPortFromPath(portPath.toUtf8().constData());
        if(port)
        { 
          DFGWrapper::ExecPortPtr portPort = DFGWrapper::ExecPortPtr::StaticCast(port);

          const char * uiRange = instPort->getExecPort()->getMetadata("uiRange");
          if(uiRange)
          {
            if(strlen(uiRange) > 0)
            {
              portPort->setMetadata("uiRange", uiRange, false);
            }
          }
          const char * uiCombo = instPort->getExecPort()->getMetadata("uiCombo");
          if(uiCombo)
          {
            if(strlen(uiCombo) > 0)
            {
              portPort->setMetadata("uiCombo", uiCombo, false);
            }
          }
          const char * uiHidden = instPort->getExecPort()->getMetadata("uiHidden");
          if(uiHidden)
          {
            if(strlen(uiHidden) > 0)
            {
              portPort->setMetadata("uiHidden", uiHidden, false);
            }
          }
        }
      }
    }

    QString portName = GraphView::lastPathSegment(portPath);
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
        addConnection(portPath, pin->path(), false, true);
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
        addConnection(pin->path(), portPath, true, false);
      }
    }
    endInteraction();
    return graph()->port(portName);
  }
  catch(FabricCore::Exception e)
  {
    endInteraction();
    logError(e.getDesc_cstr());
  }
  return NULL;
}

QString DFGController::renamePort(QString path, QString name)
{
  if(GraphView::lastPathSegment(path) == name)
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

    QString newName = command->getResult();
    emit execPortRenamed(path, newName);
    return newName;
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
    return "";
  }
  return "";
}

bool DFGController::addConnection(QString srcPath, QString dstPath, bool srcIsPin, bool dstIsPin)
{
  beginInteraction();
  try
  {
    DFGWrapper::PortPtr endPoint = getPortFromPath(dstPath.toUtf8().constData());
    if(endPoint->isValid())
    {
      if(endPoint->getNodePortType() == FabricCore::DFGPortType_IO && dstIsPin)
      {
        std::vector<GraphView::Connection*> connections = graph()->connections();
        for(size_t i=0;i<connections.size();i++)
        {
          if(((GraphView::Pin*)connections[i]->dst())->path() == dstPath)
          {
            if(!removeConnection(connections[i]->src(), connections[i]->dst()))
            {
              endInteraction();
              return false;
            }
            break;
          }
        }
      }
      else if(endPoint->isConnectedToAny())
      {
        Commands::Command * command = new DFGRemoveAllConnectionsCommand(this, 
          endPoint->getPortPath(),
          dstIsPin
        );
        if(!addCommand(command))
        {
          delete(command);
          return false;
        }
      }
    }
    Commands::Command * command = new DFGAddConnectionCommand(this, 
      srcPath.toUtf8().constData(), 
      dstPath.toUtf8().constData(),
      srcIsPin,
      dstIsPin
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
  QString srcPath;
  if(src->targetType() == GraphView::TargetType_Pin)
    srcPath = ((GraphView::Pin*)src)->path();
  else if(src->targetType() == GraphView::TargetType_Port)
    srcPath = ((GraphView::Port*)src)->path();
  QString dstPath;
  if(dst->targetType() == GraphView::TargetType_Pin)
    dstPath = ((GraphView::Pin*)dst)->path();
  else if(dst->targetType() == GraphView::TargetType_Port)
    dstPath = ((GraphView::Port*)dst)->path();
  return addConnection(srcPath, dstPath,
    src->targetType() == GraphView::TargetType_Pin,
    dst->targetType() == GraphView::TargetType_Pin
  );
}

bool DFGController::removeConnection(QString srcPath, QString dstPath, bool srcIsPin, bool dstIsPin)
{
  // filter out IO ports
  if(!srcIsPin && !dstIsPin)
  {
    if(srcPath == dstPath)
      return false;
  }

  try
  {
    Commands::Command * command = new DFGRemoveConnectionCommand(this, 
      srcPath.toUtf8().constData(), 
      dstPath.toUtf8().constData(),
      srcIsPin,
      dstIsPin
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
  QString srcPath;
  if(src->targetType() == GraphView::TargetType_Pin)
    srcPath = ((GraphView::Pin*)src)->path();
  else if(src->targetType() == GraphView::TargetType_Port)
    srcPath = ((GraphView::Port*)src)->path();
  QString dstPath;
  if(dst->targetType() == GraphView::TargetType_Pin)
    dstPath = ((GraphView::Pin*)dst)->path();
  else if(dst->targetType() == GraphView::TargetType_Port)
    dstPath = ((GraphView::Port*)dst)->path();
  return removeConnection(srcPath, dstPath,
    src->targetType() == GraphView::TargetType_Pin,
    dst->targetType() == GraphView::TargetType_Pin
  );
}

bool DFGController::removeAllConnections(QString path, bool isPin)
{
  try
  {
    Commands::Command * command = new DFGRemoveAllConnectionsCommand(this, 
      path.toUtf8().constData(), 
      isPin
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

bool DFGController::addExtensionDependency(QString extension, QString execPath, QString & errorMessage)
{
  try
  {
    m_client->loadExtension(extension.toUtf8().constData(), "", false);
    DFGWrapper::ExecutablePtr exec = getExecFromPath(execPath.toUtf8().constData());
    exec->addExtensionDependency(extension.toUtf8().constData());
  }
  catch(FabricCore::Exception e)
  {
    errorMessage = e.getDesc_cstr();
    logError(e.getDesc_cstr());
    return false;
  }
  return true;
}

bool DFGController::setCode(QString path, QString code)
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
    getBinding().execute();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }

  emit recompiled();
  return true;
}

QString DFGController::reloadCode(QString path)
{
  DFGWrapper::FuncExecutablePtr func = getFuncExecFromPath(path.toUtf8().constData());
  if(func)
  {
    std::string filePath = func->getImportPathname();
    FILE * file = fopen(filePath.c_str(), "rb");
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
  }
  return "";
}

bool DFGController::setArg(QString argName, QString dataType, QString json)
{
  beginInteraction();
  try
  {
    Commands::Command * command = new DFGSetArgCommand(this, argName, dataType, json);;
    if(addCommand(command))
    {
      // check other ports which have no value bound and set an arg on them too
      DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
      FabricCore::DFGBinding binding = exec->getDFGBinding();
      DFGWrapper::ExecPortList ports = exec->getExecPorts();

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

bool DFGController::setArg(QString argName, FabricCore::RTVal value)
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

bool DFGController::setDefaultValue(QString path, FabricCore::RTVal value)
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

bool DFGController::setDefaultValue(QString path, QString dataType, QString json)
{
  try
  {
    FabricCore::RTVal value = FabricCore::ConstructRTValFromJSON(*getClient(), dataType.toUtf8().constData(), json.toUtf8().constData());
    return setDefaultValue(path, value);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return false;
}

QString DFGController::exportJSON(QString path)
{
  try
  {
    return getBinding().exportJSON().c_str();
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  return "";
}

bool DFGController::setNodeCacheRule(QString path, FEC_DFGCacheRule rule)
{
  try
  {
    DFGWrapper::InstPtr inst = getInstFromPath(path.toUtf8().constData());
    if(inst->getCacheRule() == rule)
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

bool DFGController::moveNode(QString path, QPointF pos, bool isTopLeftPos)
{
  try
  {
    DFGWrapper::NodePtr node = getNodeFromPath(path.toUtf8().constData());
    QString metaData = "{\"x\": "+QString::number(pos.x())+", \"y\": "+QString::number(pos.y())+"}";
    node->setMetadata("uiGraphPos", metaData.toUtf8().constData(), false);
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
  return moveNode(uiNode->path(), pos, isTopLeftPos);
}

bool DFGController::zoomCanvas(float zoom)
{
  try
  {
    DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
    QString metaData = "{\"value\": "+QString::number(zoom)+"}";
    exec->setMetadata("uiGraphZoom", metaData.toUtf8().constData(), false);
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
    DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
    QString metaData = "{\"x\": "+QString::number(pan.x())+", \"y\": "+QString::number(pan.y())+"}";
    exec->setMetadata("uiGraphPan", metaData.toUtf8().constData(), false);
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
      paths.append(nodes[i]->path());
  }

  std::vector<GraphView::Node*> allNodes = graph()->nodes();
  for(unsigned int i=0;i<allNodes.size();i++)
  {
    allNodes[i]->setRow(-1);
    allNodes[i]->setCol(-1);
  }

  std::vector<GraphView::Node*> rootNodes;
  for(unsigned int i=0;i<paths.length();i++)
  {
    GraphView::Node * uiNode = graph()->nodeFromPath(paths[i]);
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
    relaxer.addNode(nodes[i]->path(), nodes[i]->topLeftGraphPos(), nodes[i]->col(), nodes[i]->row(), rect.width(), rect.height());
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

    relaxer.addSpring(srcNode->path(), dstNode->path());
  }

  relaxer.relax(50);

  for(unsigned int i=0;i<relaxer.numNodes();i++)
  {
    moveNode(relaxer.getName(i), relaxer.getPos(i), true);
  }

  return true;
}

QString DFGController::copy(QStringList paths)
{
  try
  {
    if(paths.length() == 0)
    {
      const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
      for(unsigned int i=0;i<nodes.size();i++)
        paths.append(nodes[i]->path());
    }

    DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
    DFGCopyCommand * command = new DFGCopyCommand(this, paths);
    if(!addCommand(command))
    {
      delete(command);
      return "";
    }

    return command->getJSON().c_str();
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
        GraphView::Node * uiNode = graph()->nodeFromPath(paths[i].c_str());
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
        GraphView::Node * uiNode = graph()->nodeFromPath(paths[i].c_str());
        if(!uiNode)
          continue;
        uiNode->setSelected(true);
        moveNode(paths[i].c_str(), uiNode->topLeftGraphPos() + delta, true);
        // uiNode->setTopLeftGraphPos(uiNode->topLeftGraphPos() + QPointF(10, 10));
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

QString DFGController::implodeNodes(QString desiredName, QStringList paths)
{
  beginInteraction();
  try
  {
    if(paths.length() == 0)
    {
      const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
      for(unsigned int i=0;i<nodes.size();i++)
        paths.append(nodes[i]->path());
    }

    QRectF bounds;
    for(size_t i=0;i<paths.size();i++)
    {
      GraphView::Node * uiNode = graph()->nodeFromPath(paths[i]);
      if(uiNode)
        bounds = bounds.united(uiNode->boundingRect().translated(uiNode->topLeftGraphPos()));
    }

    DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
    DFGImplodeNodesCommand * command = new DFGImplodeNodesCommand(this, desiredName, paths);
    if(!addCommand(command))
    {
      delete(command);
      endInteraction();
      return "";
    }

    QString nodePath = command->getInstName().c_str();
    moveNode(nodePath, bounds.center(), false);

    GraphView::Node * uiNode = graph()->nodeFromPath(nodePath);
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

QStringList DFGController::explodeNode(QString path)
{
  QStringList result;

  beginInteraction();
  try
  {
    if(path.length() == 0)
    {
      const std::vector<GraphView::Node*> & nodes = graph()->selectedNodes();
      for(unsigned int i=0;i<nodes.size();i++)
      {
        path = nodes[i]->path();
        break;
      }
    }

    QRectF oldBound;
    GraphView::Node * uiNode = graph()->nodeFromPath(path);
    if(uiNode)
      oldBound = oldBound.united(uiNode->boundingRect().translated(uiNode->topLeftGraphPos()));

    DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
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
    for(unsigned int i=0;i<result.length();i++)
    {
      GraphView::Node * uiNode = graph()->nodeFromPath(result[i]);
      if(uiNode)
      {
        newBounds = newBounds.united(uiNode->boundingRect().translated(uiNode->topLeftGraphPos()));
      }
    }

    QPointF delta = oldBound.center() - newBounds.center();
    for(unsigned int i=0;i<result.length();i++)
    {
      GraphView::Node * uiNode = graph()->nodeFromPath(result[i]);
      if(uiNode)
      {
        moveNode(result[i], uiNode->topLeftGraphPos() + delta, true);
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

bool DFGController::reloadExtensionDependencies(QString path)
{
  DFGWrapper::ExecutablePtr exec = getExecFromPath(path.toUtf8().constData());
  if(!exec)
    return false;

  bool result = false;
  uint32_t nbExtensions = exec->getNumExtensionDependencies();
  for(uint32_t i=0;i<nbExtensions;i++)
  {
    std::string ext = exec->getExtensionDependencyName(i);
    std::string version = exec->getExtensionDependencyVersion(i);
    try
    {
      m_client->loadExtension(ext.c_str(), version.c_str(), true);
    }
    catch(FabricCore::Exception e)
    {
      logError(e.getDesc_cstr());
      return false;
    }
    result = true;
  }

  if(result)
  {
    result = execute();
  }

  return result;
}

void DFGController::checkErrors()
{
  char const *execPath = m_view->getExecPath();
  FabricCore::DFGExec const &exec = m_view->getCoreDFGExec();

  unsigned errorCount = exec.getErrorCount();
  for(unsigned i=0;i<errorCount;i++)
  {
    std::string prefixedError = execPath;
    prefixedError += " : ";
    prefixedError += exec.getError(i);
    logError( prefixedError.c_str() );
  }

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
        for(size_t i=0;i<errors.size();i++)
        {
          if(i > 0)
            errorComposed += "\n";
          errorComposed += instExec.getError(i);
        }
      }

      logError( errorComposed.c_str() );
      if ( uiNode )
        uiNode->setError(errorComposed.c_str());
    }
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
  DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
  try
  {
    exec->getDFGBinding().execute();
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
    std::string itemPath = item->path().toUtf8().constData();
    DFGWrapper::ExecutablePtr exec = m_view->getGraph();
    std::string portOrPinPath = GraphView::relativePathSTL(exec->getExecPath(), itemPath);

    // let's assume it is pin, if there's still a node name in it
    Commands::Command * command = NULL;
    if(portOrPinPath.find('.') != std::string::npos)
    {
      command = new DFGSetDefaultValueCommand(this, itemPath.c_str(), item->value());
    }
    else
    {
      command = new DFGSetArgCommand(this, item->name().toUtf8().constData(), item->value());
    }
    if(addCommand(command))
      return;
    delete(command);
  }
  catch(FabricCore::Exception e)
  {
    logError(e.getDesc_cstr());
  }
  endInteraction();
}

bool DFGController::bindUnboundRTVals(std::string dataType)
{
  try
  {
    DFGWrapper::GraphExecutablePtr exec = m_view->getGraph();
    FabricCore::DFGBinding binding = exec->getDFGBinding();
    DFGWrapper::ExecPortList ports = exec->getExecPorts();

    bool argsHaveChanged = false;

    for(size_t i=0;i<ports.size();i++)
    {
      std::string dataTypeToCheck = dataType;
      if(dataTypeToCheck.length() == 0)
        dataTypeToCheck = ports[i]->getResolvedType();
      if(dataTypeToCheck.length() == 0)
        continue;
      else if(ports[i]->getResolvedType() != dataTypeToCheck)
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
        value = binding.getArgValue(ports[i]->getPortName());
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

      addCommand(new DFGSetArgCommand(this, ports[i]->getPortName(), dataTypeToCheck.c_str()));
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
    DFGStringResult result =
      getCoreDFGExec()->canConnectTo( pathA, pathB );
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
  Controller::populateNodeToolbar(toolbar, node);
  toolbar->addTool("node_edit", "node_edit.png");
}

DFGWrapper::NodePtr DFGController::getNodeFromPath(const std::string & path)
{
  DFGWrapper::GraphExecutablePtr graph = getGraphExec();
  // std::string relPath = GraphView::relativePathSTL(graph->getGraphPath(), path);
  std::string nodeName = path;
  int period = path.rfind('.');
  if(period != std::string::npos)
  {
    graph = getGraphExecFromPath(path.substr(0, period));
    nodeName = path.substr(period+1, path.length());
  }
  else
    graph = getGraphExec();

  return graph->getNode(nodeName.c_str());
}

DFGWrapper::InstPtr DFGController::getInstFromPath(const std::string & path)
{
  DFGWrapper::GraphExecutablePtr graph = getGraphExec();
  // std::string relPath = GraphView::relativePathSTL(graph->getGraphPath(), path);
  std::string nodeName = path;
  int period = path.rfind('.');
  if(period != std::string::npos)
  {
    graph = getGraphExecFromPath(path.substr(0, period));
    nodeName = path.substr(period+1, path.length());
  }
  else
    graph = getGraphExec();

  DFGWrapper::NodePtr node = graph->getNode(nodeName.c_str());
  if ( node->isInst() )
    return DFGWrapper::InstPtr::StaticCast( node );
  else
    return DFGWrapper::InstPtr();
}

DFGWrapper::ExecutablePtr DFGController::getExecFromPath(const std::string & path)
{
  DFGWrapper::GraphExecutablePtr graph = getGraphExec();
  if(path.length() == 0)
    return graph;

  std::string nodeName = path;
  int period = path.rfind('.');
  if(period != std::string::npos)
  {
    graph = getGraphExecFromPath(path.substr(0, period));
    nodeName = path.substr(period+1, path.length());
  }

  DFGWrapper::NodePtr node = graph->getNode(nodeName.c_str());
  if ( node->isInst() )
    return DFGWrapper::InstPtr::StaticCast( node )->getExecutable();
  else
    return DFGWrapper::ExecutablePtr();
}

DFGWrapper::ExecutablePtr DFGController::getExecFromGlobalPath(const std::string & path)
{
  DFGWrapper::GraphExecutablePtr graph = DFGWrapper::GraphExecutablePtr::StaticCast(getBinding().getExecutable());
  std::string relPath = GraphView::relativePathSTL(graph->getExecPath(), path);
  if(relPath.length() == 0)
    return graph;
  std::string nodeName = relPath;
  int period = relPath.rfind('.');
  if(period != std::string::npos)
  {
    graph = getGraphExecFromPath(path.substr(0, period));
    nodeName = path.substr(period+1, relPath.length());
  }
  else
    graph = DFGWrapper::GraphExecutablePtr::StaticCast(getBinding().getExecutable());

  DFGWrapper::NodePtr node = graph->getNode(nodeName.c_str());
  if ( node->isInst() )
    return DFGWrapper::InstPtr::StaticCast( node )->getExecutable();
  else
    return DFGWrapper::ExecutablePtr();
}

DFGWrapper::GraphExecutablePtr DFGController::getGraphExecFromPath(const std::string & path)
{
  DFGWrapper::ExecutablePtr exec = getExecFromPath(path);
  if(!exec)
    return DFGWrapper::GraphExecutablePtr();
  return DFGWrapper::GraphExecutablePtr::StaticCast(exec);
}

DFGWrapper::FuncExecutablePtr DFGController::getFuncExecFromPath(const std::string & path)
{
  DFGWrapper::ExecutablePtr exec = getExecFromPath(path);
  if(!exec)
    return DFGWrapper::FuncExecutablePtr();
  return DFGWrapper::FuncExecutablePtr::StaticCast(exec);
}

DFGWrapper::PortPtr DFGController::getPortFromPath(const std::string & path)
{
  DFGWrapper::GraphExecutablePtr graph = getGraphExec();
  return DFGWrapper::Port::Create(
    graph->getDFGBinding(),
    graph->getExecPath(),
    graph->getDFGExec(),
    path.c_str()
    );
}

void DFGController::nodeToolTriggered(FabricUI::GraphView::Node * node, QString toolName)
{
  Controller::nodeToolTriggered(node, toolName);

  if(toolName == "node_collapse")
  {
    int collapsedState = (int)node->collapsedState();
    FabricCore::Variant collapsedStateVar = FabricCore::Variant::CreateSInt32(collapsedState);
    DFGWrapper::NodePtr dfgNode = getNodeFromPath(node->path().toUtf8().constData());
    dfgNode->setMetadata("uiCollapsedState", collapsedStateVar.getJSONEncoding().getStringData(), false);
  }
  else if(toolName == "node_edit")
  {
    emit nodeEditRequested(node);
  }
}

void DFGController::bindingNotificationCallback(void * userData, char const *jsonCString, uint32_t jsonLength)
{
  if(!jsonCString)
    return;
  DFGController * ctrl = (DFGController *)userData;

  FabricCore::Variant notificationVar = FabricCore::Variant::CreateFromJSON(jsonCString, jsonLength);

  const FabricCore::Variant * descVar = notificationVar.getDictValue("desc");
  if(!descVar)
    return;

  std::string descStr = descVar->getStringData();

  // if(descStr == "argTypeChanged")
  // {
  //   ctrl->m_view->updateDataTypesOnPorts();
  // }
  if(descStr == "argChanged")
  {
    const FabricCore::Variant * nameVar = notificationVar.getDictValue("name");
    emit ctrl->argValueChanged(nameVar->getStringData());
  }
}

QStringList DFGController::getPresetPathsFromSearch(QString search, bool includePresets, bool includeNameSpaces)
{
  if(search.length() == 0)
    return QStringList();

  updatePresetPathDB();

  QStringList results;

  // [pzion 20150305] This is a little evil but avoids lots of copying

  std::string stdString = search.toUtf8().constData();

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
  if(m_presetDictsUpToDate || m_host == NULL)
    return;
  m_presetDictsUpToDate = true;

  m_presetNameSpaceDict.clear();
  m_presetPathDict.clear();
  m_presetNameSpaceDictSTL.clear();
  m_presetPathDictSTL.clear();

  std::vector<DFGWrapper::NameSpace> namespaces;
  namespaces.push_back(m_host->getRootNameSpace());

  for(size_t i=0;i<namespaces.size();i++)
  {
    m_presetNameSpaceDictSTL.push_back(namespaces[i].getPath());

    std::vector<DFGWrapper::NameSpace> childNameSpaces = namespaces[i].getNameSpaces();
    namespaces.insert(namespaces.end(), childNameSpaces.begin(), childNameSpaces.end());

    std::vector<DFGWrapper::Object> presets = namespaces[i].getPresets();
    for(size_t j=0;j<presets.size();j++)
      m_presetPathDictSTL.push_back(presets[j].getPath());
  }

  for(size_t i=0;i<m_presetNameSpaceDictSTL.size();i++)
    m_presetNameSpaceDict.add(m_presetNameSpaceDictSTL[i].c_str(), '.', m_presetNameSpaceDictSTL[i].c_str());
  for(size_t i=0;i<m_presetPathDictSTL.size();i++)
    m_presetPathDict.add(m_presetPathDictSTL[i].c_str(), '.', m_presetPathDictSTL[i].c_str());
}
