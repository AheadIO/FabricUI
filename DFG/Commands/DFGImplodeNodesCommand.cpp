// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGImplodeNodesCommand.h"
#include <DFGWrapper/Inst.h>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <assert.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGImplodeNodesCommand::DFGImplodeNodesCommand(DFGController * controller, QString desiredName, QStringList nodePaths)
: DFGCommand(controller)
{
  m_desiredName = desiredName.toUtf8().constData();
  for(unsigned int i=0;i<nodePaths.length();i++)
    m_nodePaths.push_back(nodePaths[i].toUtf8().constData());
  for(unsigned int i=0;i<m_nodePaths.size();i++)
    m_nodePathsPtr.push_back(m_nodePaths[i].c_str());
}

std::vector<std::string> DFGImplodeNodesCommand::getNodePaths() const
{
  return m_nodePaths;
}

std::string DFGImplodeNodesCommand::getDesiredName() const
{
  return m_desiredName;
}

std::string DFGImplodeNodesCommand::getInstName() const
{
  return m_instName;
}

bool DFGImplodeNodesCommand::invoke()
{
  if(m_nodePaths.size() == 0)
    return false;

  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();

  m_instName = graph.implodeNodes(m_desiredName.c_str(), m_nodePathsPtr.size(), &m_nodePathsPtr[0]);
  graph.setInstTitle(m_instName.c_str(), m_desiredName.c_str());

  return true;
}
