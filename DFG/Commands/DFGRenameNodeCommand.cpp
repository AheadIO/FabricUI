// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRenameNodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRenameNodeCommand::DFGRenameNodeCommand(DFGController * controller, char const * path, char const * title)
: DFGCommand(controller)
{
  m_nodePath = path;
  m_newTitle = title;
}

bool DFGRenameNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  switch ( graph.getNodeType( m_nodePath.c_str() ) )
  {
    case FabricCore::DFGNodeType_Inst:
      graph.setInstTitle(m_nodePath.c_str(), m_newTitle.c_str());
      break;

    case FabricCore::DFGNodeType_User:
      graph.setNodeMetadata(
        m_nodePath.c_str(),
        "uiTitle",
        m_newTitle.c_str(),
        true
        );
      break;

    default:
      break;
  }
  return true;
}

char const * DFGRenameNodeCommand::getPath() const
{
  return m_nodePath.c_str();
}

char const * DFGRenameNodeCommand::getTitle() const
{
  return m_newTitle.c_str();
}
