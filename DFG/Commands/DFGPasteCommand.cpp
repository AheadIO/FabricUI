// Pasteright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGPasteCommand.h"
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGPasteCommand::DFGPasteCommand(DFGController * controller, QString json)
: DFGCommand(controller)
{
  m_json = json.toUtf8().constData();
  if(m_json.length() == 0)
  {
    QClipboard *clipboard = QApplication::clipboard();
    m_json = clipboard->text().toUtf8().constData();
  }
}

std::string DFGPasteCommand::getJSON() const
{
  return m_json;
}

std::vector<std::string> DFGPasteCommand::getNodePaths() const
{
  return m_nodePaths;
}

bool DFGPasteCommand::invoke()
{
  if(m_json.length() == 0)
    return false;

  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::GraphExecutablePtr graph = ctrl->getGraphExec();

  std::string result = graph->importNodesJSON(m_json.c_str());
  FabricCore::Variant resultVar = FabricCore::Variant::CreateFromJSON(result.c_str());
  if(resultVar.isArray())
  {
    m_nodePaths.clear();

    for(unsigned int i=0;i<resultVar.getArraySize();i++)
    {
      const FabricCore::Variant * singleResultVar = resultVar.getArrayElement(i);
      if(singleResultVar->isString())
        m_nodePaths.push_back(singleResultVar->getStringData());
    }
  }

  return true;
}

bool DFGPasteCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();
}

bool DFGPasteCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();
}
