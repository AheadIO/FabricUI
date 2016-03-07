// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetCode__
#define __UI_DFG_DFGUICmd_SetCode__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetCode
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetCode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString code
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_code( code )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetCode"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::CStrRef code,
    unsigned &coreUndoCount
    );

private:

  QString m_code;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetCode__
