// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetArgType__
#define __UI_DFG_DFGUICmd_SetArgType__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Binding.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetArgType
  : public DFGUICmd_Binding
{
public:

  DFGUICmd_SetArgType(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef argName,
    FTL::StrRef typeName
    )
    : DFGUICmd_Binding( binding )
    , m_argName( argName )
    , m_typeName( typeName )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetArgType"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_argName;
  std::string m_typeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetArgType__
