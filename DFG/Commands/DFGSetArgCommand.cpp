// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGSetArgCommand.h"

#include <CodeCompletion/KLTypeDesc.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetArgCommand::DFGSetArgCommand(DFGController * controller, char const * argName, char const * dataType, char const * json)
: DFGCommand(controller)
{
  m_argName = argName;
  m_dataType = dataType;
  m_json = json;
}

DFGSetArgCommand::DFGSetArgCommand(DFGController * controller, char const * argName, FabricCore::RTVal value)
: DFGCommand(controller)
{
  m_argName = argName;
  m_value = value;
}

bool DFGSetArgCommand::invoke()
{
  if(!m_value.isValid() && m_dataType.length() == 0)
    return false;

  DFGController* ctrl = (DFGController*)controller();
  FabricCore::DFGBinding binding = ctrl->getCoreDFGBinding();

  CodeCompletion::KLTypeDesc typeDesc(m_dataType);
  std::string baseType = typeDesc.getBaseType();

  if(!m_value.isValid())
  {
    FabricCore::Client client = ctrl->getClient();
    
    if(m_json.length() > 0)
      m_value = FabricCore::ConstructRTValFromJSON(client, m_dataType.c_str(), m_json.c_str());
    else if(typeDesc.isVariableArray())
      m_value = FabricCore::RTVal::ConstructVariableArray(client, baseType.c_str());
    else if(typeDesc.isExternalArray())
      m_value = FabricCore::RTVal::ConstructExternalArray(client, baseType.c_str(), 0, 0);
    else if(baseType == "Boolean")
      m_value = FabricCore::RTVal::ConstructBoolean(client, false);
    else if(baseType == "SInt16")
      m_value = FabricCore::RTVal::ConstructSInt16(client, 0);
    else if(baseType == "Integer" || baseType == "SInt32")
      m_value = FabricCore::RTVal::ConstructSInt32(client, 0);
    else if(baseType == "SInt64")
      m_value = FabricCore::RTVal::ConstructSInt64(client, 0);
    else if(baseType == "UInt16")
      m_value = FabricCore::RTVal::ConstructUInt16(client, 0);
    else if(baseType == "Byte" || baseType == "UInt8")
      m_value = FabricCore::RTVal::ConstructUInt8(client, 0);
    else if(baseType == "Size" || baseType == "Count" || baseType == "Index" || baseType == "UInt32")
      m_value = FabricCore::RTVal::ConstructUInt32(client, 0);
    else if(baseType == "UInt64")
      m_value = FabricCore::RTVal::ConstructUInt64(client, 0);
    else if(baseType == "Scalar" || baseType == "Float32")
      m_value = FabricCore::RTVal::ConstructFloat32(client, 0.0f);
    else if(baseType == "Float64")
      m_value = FabricCore::RTVal::ConstructFloat64(client, 0.0f);
    else if(baseType == "String")
      m_value = FabricCore::RTVal::ConstructString(client, "");
    else if(FabricCore::GetRegisteredTypeIsStruct(client, baseType.c_str()))
      m_value = FabricCore::RTVal::Construct(client, m_dataType.c_str(), 0, 0);
    else if(FabricCore::GetRegisteredTypeIsObject(client, baseType.c_str()))
      m_value = FabricCore::RTVal::Create(client, m_dataType.c_str(), 0, 0);
    else
      return false;
  }

  binding.setArgValue(m_argName.c_str(), m_value);
  return true;
}

const char * DFGSetArgCommand::getArgName() const
{
  return m_argName.c_str();
}

const char * DFGSetArgCommand::getDataType() const
{
  return m_dataType.c_str();
}

const char * DFGSetArgCommand::getJSON() const
{
  return m_json.c_str();
}

FabricCore::RTVal DFGSetArgCommand::getValue() const
{
  return m_value;
}
