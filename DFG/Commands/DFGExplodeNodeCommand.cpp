// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGExplodeNodeCommand.h"
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGExplodeNodeCommand::DFGExplodeNodeCommand(DFGController * controller, char const * nodePath)
: DFGCommand(controller)
{
  m_nodePath = nodePath;
}

char const * DFGExplodeNodeCommand::getNodePath() const
{
  return m_nodePath.c_str();
}

std::vector<std::string> DFGExplodeNodeCommand::getNodeNames() const
{
  return m_nodeNames;
}

bool DFGExplodeNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();

  m_nodeNames.clear();

  std::string resultStr = graph.explodeNode(m_nodePath.c_str()).getCString();
  if(resultStr.length() == 0)
    return false;

  FabricCore::Variant resultVar = FabricCore::Variant::CreateFromJSON(resultStr.c_str());
  if(resultVar.isArray())
  {
    for(uint32_t i=0;i<resultVar.getArraySize();i++)
    {
      char const* nodeName = resultVar.getArrayElement(i)->getStringData();
      m_nodeNames.push_back(nodeName);
    }
  }

  return true;
}

