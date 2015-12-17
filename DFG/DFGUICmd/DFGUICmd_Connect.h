// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Connect__
#define __UI_DFG_DFGUICmd_Connect__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Connect
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_Connect(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString srcPath,
    QString dstPath
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_srcPath( srcPath.trimmed() )
    , m_dstPath( dstPath.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("Connect"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invokeAdd(
    FTL::CStrRef srcPath,
    FTL::CStrRef dstPath,
    unsigned &coreUndoCount
    );

private:

  QString m_srcPath;
  QString m_dstPath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Connect__
