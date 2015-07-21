// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddInstFromPreset_QUndo__
#define __UI_DFG_DFGUICmd_AddInstFromPreset_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddNode_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddInstFromPreset_QUndo
  : public DFGUICmd_AddNode_QUndo
{
public:

  DFGUICmd_AddInstFromPreset_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef presetPath,
    QPointF pos
    )
    : DFGUICmd_AddNode_QUndo(
      desc,
      binding,
      execPath,
      exec,
      presetPath,
      pos
      )
    {}

protected:
  
  virtual FTL::CStrRef invokeAdd(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef presetPath,
    unsigned &coreUndoCount
    )
  {
    FTL::CStrRef result = exec.addInstFromPreset( presetPath.c_str() );
    ++coreUndoCount;
    return result;
  }
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddInstFromPreset_QUndo__
