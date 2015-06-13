// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddConnection_QUndo__
#define __UI_DFG_DFGUICmd_AddConnection_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddConnection_QUndo
  : public DFGUICmd_QUndo
{
public:

  DFGUICmd_AddConnection_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef srcPath,
    FTL::StrRef dstPath
    )
    : DFGUICmd_QUndo( desc, binding, execPath, exec )
    , m_srcPath( srcPath )
    , m_dstPath( dstPath )
    {}

protected:
  
  virtual void invoke(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    exec.connectTo(
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

#endif // __UI_DFG_DFGUICmd_AddConnection_QUndo__
