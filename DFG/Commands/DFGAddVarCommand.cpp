// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddVarCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddVarCommand::DFGAddVarCommand(
  DFGController * controller,
  FTL::StrRef name,
  FTL::StrRef dataType,
  FTL::StrRef extDep,
  QPointF pos
  )
  : DFGCommand(controller)
  , m_varName(name)
  , m_dataType(dataType)
  , m_extDep(extDep)
  , m_pos(pos)
{
}

char const * DFGAddVarCommand::getVarName() const
{
  return m_varName.c_str();
}

char const * DFGAddVarCommand::getDataType() const
{
  return m_dataType.c_str();
}

char const * DFGAddVarCommand::getExtDep() const
{
  return m_extDep.c_str();
}

char const * DFGAddVarCommand::getNodePath() const
{
  return m_nodePath.c_str();
}

QPointF DFGAddVarCommand::getPos() const
{
  return m_pos;
}

GraphView::Node * DFGAddVarCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->graph())
    return ctrl->graph()->nodeFromPath(m_nodePath.c_str());
  return NULL;
}

bool DFGAddVarCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  if(m_extDep.length() > 0)
  {
    FabricCore::Client client = ctrl->getClient();
    client.loadExtension(m_extDep.c_str(), "", false);
  }
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  m_nodePath = graph.addVar(m_varName.c_str(), m_dataType.c_str(), m_extDep.c_str());
  ctrl->moveNode(m_nodePath.c_str(), m_pos, false);
  return true;
}
