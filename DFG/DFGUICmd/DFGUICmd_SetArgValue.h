// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetArgValue__
#define __UI_DFG_DFGUICmd_SetArgValue__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Binding.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetArgValue
  : public DFGUICmd_Binding
{
public:

  DFGUICmd_SetArgValue(
    FabricCore::DFGBinding const &binding,
    QString argName,
    FabricCore::RTVal const &value
    )
    : DFGUICmd_Binding( binding )
    , m_argName( argName.trimmed() )
    , m_value( value )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetArgValue"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::CStrRef argName,
    unsigned &coreUndoCount
    );

private:

  QString m_argName;
  FabricCore::RTVal m_value;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetArgValue__
