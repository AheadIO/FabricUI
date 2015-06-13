// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGSetDefaultValueCommand.h"

#include <CodeCompletion/KLTypeDesc.h>
#include <FabricUI/GraphView/Graph.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetDefaultValueCommand::DFGSetDefaultValueCommand(DFGController * controller, char const * path, FabricCore::RTVal value)
: DFGCommand(controller)
{
  m_path = path;
  m_value = value;
  m_dataType = m_value.getTypeName().getStringCString();
  m_json = m_value.getJSON().getStringCString();
}

bool DFGSetDefaultValueCommand::invoke()
{
  if(!m_value.isValid())
    return false;

  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getExec();

  if(m_path.find('.') != std::string::npos)
    graph.setPortDefaultValue(m_path.c_str(), m_value);
  else
    graph.setPortDefaultValue(m_path.c_str(), m_value);

  return true;
}

const char * DFGSetDefaultValueCommand::getPath() const
{
  return m_path.c_str();
}

const char * DFGSetDefaultValueCommand::getDataType() const
{
  return m_dataType.c_str();
}

const char * DFGSetDefaultValueCommand::getJSON() const
{
  return m_json.c_str();
}

FabricCore::RTVal DFGSetDefaultValueCommand::getValue() const
{
  return m_value;
}
