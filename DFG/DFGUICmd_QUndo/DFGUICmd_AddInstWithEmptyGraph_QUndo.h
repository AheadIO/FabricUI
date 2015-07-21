// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddInstWithEmptyGraph_QUndo__
#define __UI_DFG_DFGUICmd_AddInstWithEmptyGraph_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddNode_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddInstWithEmptyGraph_QUndo
  : public DFGUICmd_AddNode_QUndo
{
public:

  DFGUICmd_AddInstWithEmptyGraph_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef title,
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
    FTL::CStrRef result = exec.addInstWithNewGraph( title.c_str() );
    ++coreUndoCount;
    return result;
  }
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddInstWithEmptyGraph_QUndo__
