// Pasteright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGPasteCommand.h"
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGPasteCommand::DFGPasteCommand(DFGController * controller, char const * json)
: DFGCommand(controller)
{
  m_json = json;
  if(m_json.length() == 0)
  {
    QClipboard *clipboard = QApplication::clipboard();
    m_json = clipboard->text().toUtf8().constData();
  }
}

char const * DFGPasteCommand::getJSON() const
{
  return m_json.c_str();
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
  FabricCore::DFGExec graph = ctrl->getExec();

  std::string result = graph.importNodesJSON(m_json.c_str()).getCString();
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
