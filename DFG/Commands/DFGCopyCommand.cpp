// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGCopyCommand.h"
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGCopyCommand::DFGCopyCommand(DFGController * controller, QStringList nodePaths)
: DFGCommand(controller)
{
  for(int i=0;i<nodePaths.length();i++)
    m_nodePaths.push_back(nodePaths[i].toUtf8().constData());
  for(unsigned int i=0;i<m_nodePaths.size();i++)
    m_nodePathsPtr.push_back(m_nodePaths[i].c_str());
}

std::vector<std::string> DFGCopyCommand::getNodePaths() const
{
  return m_nodePaths;
}

std::string DFGCopyCommand::getJSON() const
{
  return m_json;
}

bool DFGCopyCommand::invoke()
{
  if(m_nodePaths.size() == 0)
    return false;

  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  m_json = graph.exportNodesJSON(m_nodePathsPtr.size(), &m_nodePathsPtr[0]).getCString();

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(m_json.c_str());

  return true;
}

bool DFGCopyCommand::undo()
{
  return true;
}

bool DFGCopyCommand::redo()
{
  return invoke();
}
