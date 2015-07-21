// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddInstWithEmptyFunc_QUndo__
#define __UI_DFG_DFGUICmd_AddInstWithEmptyFunc_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddNode_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddInstWithEmptyFunc_QUndo
  : public DFGUICmd_AddNode_QUndo
{
public:

  DFGUICmd_AddInstWithEmptyFunc_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef title,
    FTL::StrRef initialCode,
    QPointF pos
    )
    : DFGUICmd_AddNode_QUndo(
      desc,
      binding,
      execPath,
      exec,
      title,
      pos
      )
    {}

protected:
  
  virtual FTL::CStrRef invokeAdd(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef title,
    unsigned &coreUndoCount
    )
  {
    FTL::CStrRef result = exec.addInstWithNewFunc( title.c_str() );
    ++coreUndoCount;

    FabricCore::DFGExec subExec = exec.getSubExec( result.c_str() );
    subExec.setCode( m_initialCode.c_str() );
    ++coreUndoCount;

    return result;
  }

private:

  std::string m_initialCode;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddInstWithEmptyFunc_QUndo__
