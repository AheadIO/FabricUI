// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGSetDefaultValueCommand.h"

#include <CodeCompletion/KLTypeDesc.h>
#include <FabricUI/GraphView/Graph.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetDefaultValueCommand::DFGSetDefaultValueCommand(DFGController * controller, char const * pathFromRoot, FabricCore::RTVal value)
: DFGCommand(controller)
{
  m_path = pathFromRoot;
  m_value = value;
  m_dataType = m_value.getTypeName().getStringCString();
  m_json = m_value.getJSON().getStringCString();
}

bool DFGSetDefaultValueCommand::invoke()
{
  if(!m_value.isValid())
    return false;

  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec rootExec = ctrl->getCoreDFGBinding().getExec();
  FabricCore::DFGExec parentExec = rootExec;

  std::string path = m_path;

  // for paths with more than two segments,
  // get the corresponding subexec
  // @pzion: should setPortDefaultValue / getPortDefaultValue
  // automatically do this?
  size_t posA = path.find('.');
  size_t posB = path.rfind('.');
  while(posA != std::string::npos && posA != posB)
  {
    std::string left = path.substr(0, posA);
    path = path.substr(posA+1);
    parentExec = parentExec.getSubExec(left.c_str());

    posA = path.find('.');
    posB = path.rfind('.');
  }

  parentExec.setPortDefaultValue(path.c_str(), m_value);

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
