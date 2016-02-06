//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUIUtil.h>
#include <FabricServices/CodeCompletion/KLTypeDesc.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

FabricCore::RTVal DFGCreateDefaultValue(
  FabricCore::Context context,
  FTL::CStrRef typeName
  )
{
  FabricServices::CodeCompletion::KLTypeDesc typeDesc( typeName );
  std::string baseType = typeDesc.getBaseType();

  FabricCore::RTVal value;
  if(typeDesc.isVariableArray())
    value = FabricCore::RTVal::ConstructVariableArray(context, baseType.c_str());
  else if(typeDesc.isExternalArray())
    value = FabricCore::RTVal::ConstructExternalArray(context, baseType.c_str(), 0, 0);
  else if(baseType == "Boolean")
    value = FabricCore::RTVal::ConstructBoolean(context, false);
  else if(baseType == "SInt16")
    value = FabricCore::RTVal::ConstructSInt16(context, 0);
  else if(baseType == "Integer" || baseType == "SInt32")
    value = FabricCore::RTVal::ConstructSInt32(context, 0);
  else if(baseType == "SInt64")
    value = FabricCore::RTVal::ConstructSInt64(context, 0);
  else if(baseType == "UInt16")
    value = FabricCore::RTVal::ConstructUInt16(context, 0);
  else if(baseType == "Byte" || baseType == "UInt8")
    value = FabricCore::RTVal::ConstructUInt8(context, 0);
  else if(baseType == "Size" || baseType == "Count" || baseType == "Index" || baseType == "UInt32")
    value = FabricCore::RTVal::ConstructUInt32(context, 0);
  else if(baseType == "UInt64")
    value = FabricCore::RTVal::ConstructUInt64(context, 0);
  else if(baseType == "Scalar" || baseType == "Float32")
    value = FabricCore::RTVal::ConstructFloat32(context, 0.0f);
  else if(baseType == "Float64")
    value = FabricCore::RTVal::ConstructFloat64(context, 0.0f);
  else if(baseType == "String")
    value = FabricCore::RTVal::ConstructString(context, "");
  else if(FabricCore::GetRegisteredTypeIsStruct(context, baseType.c_str()))
    value = FabricCore::RTVal::Construct(context, typeName.c_str(), 0, 0);
  else if(FabricCore::GetRegisteredTypeIsObject(context, baseType.c_str()))
    value = FabricCore::RTVal::Create(context, typeName.c_str(), 0, 0);
  return value;
}

FABRIC_UI_DFG_NAMESPACE_END
