// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RemovePort_QUndo__
#define __UI_DFG_DFGUICmd_RemovePort_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RemovePort_QUndo
  : public DFGUICmd_QUndo
{
public:

  DFGUICmd_RemovePort_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef portName
    )
    : DFGUICmd_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_portName( portName )
    {}

protected:
  
  virtual void invoke(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_RemovePort(
      binding,
      execPath,
      exec,
      m_portName,
      coreUndoCount
      );
  }

private:

  std::string m_portName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemovePort_QUndo__
