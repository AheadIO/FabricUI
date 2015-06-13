// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddPort_QUndo__
#define __UI_DFG_DFGUICmd_AddPort_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddPort_QUndo
  : public DFGUICmd_QUndo
{
public:

  DFGUICmd_AddPort_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredPortName,
    FabricCore::DFGPortType portType,
    FTL::CStrRef typeSpec,
    FTL::CStrRef portToConnectWith
    )
    : DFGUICmd_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_desiredPortName( desiredPortName )
    , m_portType( portType )
    , m_typeSpec( typeSpec )
    , m_portToConnectWith( portToConnectWith )
    {}

  FTL::CStrRef getPortName() const
    { return m_portName; }

protected:
  
  virtual void invoke(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    m_portName =
      DFGUIPerform_AddPort(
        binding,
        execPath,
        exec,
        m_desiredPortName,
        m_portType,
        m_typeSpec,
        m_portToConnectWith,
        coreUndoCount
        );
  }

private:

  std::string m_desiredPortName;
  FabricCore::DFGPortType m_portType;
  std::string m_typeSpec;
  std::string m_portToConnectWith;
  
  std::string m_portName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddPort_QUndo__
