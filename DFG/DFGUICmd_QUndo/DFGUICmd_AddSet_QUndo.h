//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUICmd_AddSet_QUndo__
#define __UI_DFG_DFGUICmd_AddSet_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddNode_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddSet_QUndo
  : public DFGUICmd_AddNode_QUndo
{
public:

  DFGUICmd_AddSet_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef desiredNodeName,
    FTL::StrRef varPath,
    QPointF pos
    )
    : DFGUICmd_AddNode_QUndo(
      desc,
      binding,
      execPath,
      exec,
      desiredNodeName,
      pos
      )
    , m_varPath( varPath )
    {}

protected:
  
  virtual FTL::CStrRef invokeAdd(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    unsigned &coreUndoCount
    )
  {
    FTL::CStrRef result = exec.addSet(
      desiredNodeName.c_str(),
      m_varPath.c_str()
      );
    ++coreUndoCount;
    return result;
  }

private:

  std::string m_varPath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddSet_QUndo__
