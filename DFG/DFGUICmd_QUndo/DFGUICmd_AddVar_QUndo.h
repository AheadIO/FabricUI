// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddVar_QUndo__
#define __UI_DFG_DFGUICmd_AddVar_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddNode_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddVar_QUndo
  : public DFGUICmd_AddNode_QUndo
{
public:

  DFGUICmd_AddVar_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef desiredNodeName,
    FTL::StrRef dataType,
    FTL::StrRef extDep,
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
    , m_dataType( dataType )
    , m_extDep( extDep )
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
    FTL::CStrRef result = exec.addVar(
      desiredNodeName.c_str(),
      m_dataType.c_str(),
      m_extDep.c_str()
      );
    ++coreUndoCount;
    return result;
  }

private:

  std::string m_dataType;
  std::string m_extDep;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddVar_QUndo__
