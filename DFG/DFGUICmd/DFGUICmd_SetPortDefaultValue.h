// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetPortDefaultValue__
#define __UI_DFG_DFGUICmd_SetPortDefaultValue__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetPortDefaultValue
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetPortDefaultValue(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portPath,
    FabricCore::RTVal const &value
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_portPath( portPath )
    , m_value( value )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetPortDefaultValue"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::CStrRef portPath,
    unsigned &coreUndoCount
    );

private:

  QString m_portPath;
  FabricCore::RTVal m_value;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetPortDefaultValue__
