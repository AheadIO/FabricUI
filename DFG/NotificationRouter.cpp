// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NotificationRouter.h"
#include "DFGGraph.h"
#include "DFGController.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

NotificationRouter::NotificationRouter(
  FabricCore::DFGBinding coreDFGBinding,
  FabricCore::DFGExec coreDFGExec
  )
  : m_coreDFGBinding( coreDFGBinding )
  , m_coreDFGExec( coreDFGExec )
  , m_coreDFGView(
    coreDFGExec.createView( &Callback, this )
    )
  , m_controller(NULL)
{
}

void NotificationRouter::callback( FTL::StrRef jsonStr )
{
  onNotification(jsonStr);

  FabricCore::Variant notificationVar = FabricCore::Variant::CreateFromJSON(jsonStr.data(), jsonStr.size());
  const FabricCore::Variant * descVar = notificationVar.getDictValue("desc");
  std::string descStr = descVar->getStringData();

  if(descStr == "nodeInserted")
  {
    const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
    char const * nodePath = nodePathVar->getStringData();
    onNodeInserted(m_coreDFGExec, nodePath);
  }
  // else if(descStr == "nodeRemoved")
  // {
  //   const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
  //   NodePtr node = Node::Create(
  //     dfgBinding, execPath, dfgExec, nodePathVar->getStringData()
  //     );
  //   onNodeRemoved(node);
  // }
  else if(descStr == "nodePortInserted")
  {
    const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
    onNodePortInserted(m_coreDFGExec, nodePortPathVar->getStringData());
  }
  // else if(descStr == "nodePortRemoved")
  // {
  //   const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
  //   NodePortPtr nodePort = NodePort::Create(
  //     dfgBinding, execPath, dfgExec, nodePortPathVar->getStringData()
  //     );
  //   onNodePortRemoved(nodePort);
  // }
  // else if(descStr == "execPortInserted")
  // {
  //   const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
  //   ExecPortPtr port = ExecPort::Create(
  //     dfgBinding, execPath, dfgExec, portPathVar->getStringData()
  //     );
  //   onExecPortInserted(port);
  // }
  // else if(descStr == "execPortRemoved")
  // {
  //   const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
  //   ExecPortPtr port = ExecPort::Create(
  //     dfgBinding, execPath, dfgExec, portPathVar->getStringData()
  //     );
  //   onExecPortRemoved(port);
  // }
  // else if(descStr == "portsConnected")
  // {
  //   const FabricCore::Variant * srcPortPathVar = notificationVar.getDictValue("srcPath");
  //   const FabricCore::Variant * dstPortPathVar = notificationVar.getDictValue("dstPath");
  //   PortPtr src = Port::Create(
  //     dfgBinding, execPath, dfgExec, srcPortPathVar->getStringData()
  //     );
  //   PortPtr dst = Port::Create(
  //     dfgBinding, execPath, dfgExec, dstPortPathVar->getStringData()
  //     );
  //   onPortsConnected(src, dst);
  // }
  // else if(descStr == "portsDisconnected")
  // {
  //   const FabricCore::Variant * srcPortPathVar = notificationVar.getDictValue("srcPath");
  //   const FabricCore::Variant * dstPortPathVar = notificationVar.getDictValue("dstPath");
  //   PortPtr src = Port::Create(
  //     dfgBinding, execPath, dfgExec, srcPortPathVar->getStringData()
  //     );
  //   PortPtr dst = Port::Create(
  //     dfgBinding, execPath, dfgExec, dstPortPathVar->getStringData()
  //     );
  //   onPortsDisconnected(src, dst);
  // }
  else if(descStr == "nodeMetadataChanged")
  {
    const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
    const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
    const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
    onNodeMetadataChanged(m_coreDFGExec, nodePathVar->getStringData(), keyVar->getStringData(), valueVar->getStringData());
  }
  else if(descStr == "nodeTitleChanged")
  {
    const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
    const FabricCore::Variant * titleVar = notificationVar.getDictValue("title");
    onNodeTitleChanged(m_coreDFGExec, nodePathVar->getStringData(), titleVar->getStringData());
  }
  // else if(descStr == "execPortRenamed")
  // {
  //   const FabricCore::Variant * oldPathVar = notificationVar.getDictValue("oldPath");
  //   const FabricCore::Variant * newPathVar = notificationVar.getDictValue("newPath");
  //   std::string oldPath = oldPathVar->getStringData();
  //   std::string newPath = newPathVar->getStringData();
  //   ExecPortPtr port = ExecPort::Create(
  //     dfgBinding, execPath, dfgExec, newPath.c_str()
  //     );
  //   onExecPortRenamed(port, oldPath.c_str());
  // }
  // else if(descStr == "nodePortRenamed")
  // {
  //   const FabricCore::Variant * oldPathVar = notificationVar.getDictValue("oldPath");
  //   const FabricCore::Variant * newPathVar = notificationVar.getDictValue("newPath");
  //   std::string oldPath = oldPathVar->getStringData();
  //   if(oldPath.rfind('.') != std::string::npos)
  //     oldPath = oldPath.substr(oldPath.find('.')+1, oldPath.length());
  //   NodePortPtr nodePort = NodePort::Create(
  //     dfgBinding, execPath, dfgExec, newPathVar->getStringData()
  //     );
  //   onNodePortRenamed(nodePort, oldPath.c_str());
  // }
  // else if(descStr == "execMetadataChanged")
  // {
  //   const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
  //   const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
  //   onExecMetadataChanged(m_exec, keyVar->getStringData(), valueVar->getStringData());
  // }
  // else if(descStr == "extDepAdded")
  // {
  //   const FabricCore::Variant * nameVar = notificationVar.getDictValue("name");
  //   const FabricCore::Variant * versionRangeVar = notificationVar.getDictValue("versionRange");
  //   onExtDepAdded(nameVar->getStringData(), versionRangeVar->getStringData());
  // }
  // else if(descStr == "extDepRemoved")
  // {
  //   const FabricCore::Variant * nameVar = notificationVar.getDictValue("name");
  //   const FabricCore::Variant * versionRangeVar = notificationVar.getDictValue("versionRange");
  //   onExtDepRemoved(nameVar->getStringData(), versionRangeVar->getStringData());
  // }
  // else if(descStr == "nodeCacheRuleChanged")
  // {
  //   const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * cacheRuleVar = notificationVar.getDictValue("cacheRule");
  //   onNodeCacheRuleChanged(nodePathVar->getStringData(), cacheRuleVar->getStringData());
  // }
  // else if(descStr == "execCacheRuleChanged")
  // {
  //   const FabricCore::Variant * execPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * cacheRuleVar = notificationVar.getDictValue("cacheRule");
  //   onExecCacheRuleChanged(execPathVar->getStringData(), cacheRuleVar->getStringData());
  // }
  // else if(descStr == "portResolvedTypeChanged")
  // {
  //   const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * resolvedTypeVar = notificationVar.getDictValue("resolvedType");
  //   ExecPortPtr port = ExecPort::Create(
  //     dfgBinding, execPath, dfgExec, portPathVar->getStringData()
  //     );
  //   if(resolvedTypeVar->isString())
  //     onExecPortResolvedTypeChanged(port, resolvedTypeVar->getStringData());
  //   else
  //     onExecPortResolvedTypeChanged(port, "");
  // }
  // else if(descStr == "portTypeSpecChanged")
  // {
  //   const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * typeSpecVar = notificationVar.getDictValue("typeSpec");
  //   ExecPortPtr port = ExecPort::Create(
  //     dfgBinding, execPath, dfgExec, portPathVar->getStringData()
  //     );
  //   if(typeSpecVar->isString())
  //     onExecPortTypeSpecChanged(port, typeSpecVar->getStringData());
  //   else
  //     onExecPortTypeSpecChanged(port, "");
  // }
  // else if(descStr == "nodePortResolvedTypeChanged")
  // {
  //   const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * resolvedTypeVar = notificationVar.getDictValue("resolvedType");
  //   NodePortPtr nodePort = NodePort::Create(
  //     dfgBinding, execPath, dfgExec, nodePortPathVar->getStringData()
  //     );
  //   if(resolvedTypeVar->isString())
  //     onNodePortResolvedTypeChanged(nodePort, resolvedTypeVar->getStringData());
  //   else
  //     onNodePortResolvedTypeChanged(nodePort, "");
  // }
  // else if(descStr == "portMetadataChanged")
  // {
  //   const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
  //   const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
  //   ExecPortPtr port = ExecPort::Create(
  //     dfgBinding, execPath, dfgExec, portPathVar->getStringData()
  //     );
  //   if(valueVar->isString())
  //     onExecPortMetadataChanged(port, keyVar->getStringData(), valueVar->getStringData());
  //   else
  //     onExecPortMetadataChanged(port, keyVar->getStringData(), "");
  // }
  // else if(descStr == "nodePortMetadataChanged")
  // {
  //   const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
  //   const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
  //   NodePortPtr nodePort = NodePort::Create(
  //     dfgBinding, execPath, dfgExec, nodePortPathVar->getStringData()
  //     );
  //   if(valueVar->isString())
  //     onNodePortMetadataChanged(nodePort, keyVar->getStringData(), valueVar->getStringData());
  //   else
  //     onNodePortMetadataChanged(nodePort, keyVar->getStringData(), "");
  // }
  // else if(descStr == "nodePortTypeChanged")
  // {
  //   const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * newPortTypeVar = notificationVar.getDictValue("newPortType");
  //   if(newPortTypeVar->isString())
  //   {
  //     std::string newPortType = newPortTypeVar->getStringData();
  //     NodePortPtr nodePort = NodePort::Create(
  //       dfgBinding, execPath, dfgExec, nodePortPathVar->getStringData()
  //       );
  //     if(newPortType == "In")
  //       onNodePortTypeChanged(nodePort, FabricCore::DFGPortType_In);
  //     else if(newPortType == "Out")
  //       onNodePortTypeChanged(nodePort, FabricCore::DFGPortType_Out);
  //     else if(newPortType == "IO")
  //       onNodePortTypeChanged(nodePort, FabricCore::DFGPortType_IO);
  //   }
  // }
  // else if(descStr == "execPortOutsidePortTypeChanged")
  // {
  //   const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
  //   const FabricCore::Variant * newPortTypeVar = notificationVar.getDictValue("newPortType");
  //   if(newPortTypeVar->isString())
  //   {
  //     std::string newPortType = newPortTypeVar->getStringData();
  //     ExecPortPtr port = ExecPort::Create(
  //       dfgBinding, execPath, dfgExec, portPathVar->getStringData()
  //       );
  //     if(newPortType == "In")
  //       onExecPortTypeChanged(port, FabricCore::DFGPortType_In);
  //     else if(newPortType == "Out")
  //       onExecPortTypeChanged(port, FabricCore::DFGPortType_Out);
  //     else if(newPortType == "IO")
  //       onExecPortTypeChanged(port, FabricCore::DFGPortType_IO);
  //   }
  // }
  else
  {
    printf("View::callback: Unhandled desc '%s', '%s'\n", descStr.c_str(), jsonStr.data());
  }
}
