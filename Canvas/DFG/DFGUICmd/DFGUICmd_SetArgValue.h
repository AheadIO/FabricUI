// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetArgValue__
#define __UI_DFG_DFGUICmd_SetArgValue__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Binding.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetArgValue
  : public DFGUICmd_Binding
{
public:

  DFGUICmd_SetArgValue(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef argName,
    FabricCore::RTVal const &value
    )
    : DFGUICmd_Binding( binding )
    , m_argName( argName )
    , m_value( value )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetArgValue"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_argName;
  FabricCore::RTVal m_value;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetArgValue__
