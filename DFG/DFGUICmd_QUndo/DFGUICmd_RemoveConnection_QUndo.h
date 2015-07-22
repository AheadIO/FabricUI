// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RemoveConnection_QUndo__
#define __UI_DFG_DFGUICmd_RemoveConnection_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RemoveConnection_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_RemoveConnection_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef srcPath,
    FTL::StrRef dstPath
    )
    : DFGUICmd_Exec_QUndo( desc, binding, execPath, exec )
    , m_srcPath( srcPath )
    , m_dstPath( dstPath )
    {}

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    exec.disconnectFrom(
      m_srcPath.c_str(),
      m_dstPath.c_str()
      );
    ++coreUndoCount;
  }

private:

  std::string m_srcPath;
  std::string m_dstPath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemoveConnection_QUndo__
