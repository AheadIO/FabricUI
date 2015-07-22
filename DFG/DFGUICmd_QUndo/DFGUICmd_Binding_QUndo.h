// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Binding_QUndo__
#define __UI_DFG_DFGUICmd_Binding_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Binding_QUndo : public DFGUICmd_QUndo
{
public:

  DFGUICmd_Binding_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding
    )
    : DFGUICmd_QUndo(
      desc,
      binding.getHost()
      )
    , m_binding( binding )
    {}

protected:

  virtual void invokeForHost(
    FabricCore::DFGHost &host,
    unsigned &coreUndoCount
    )
  {
    invokeForBinding( m_binding, coreUndoCount );
  }

  virtual void invokeForBinding(
    FabricCore::DFGBinding &binding,
    unsigned &coreUndoCount
    ) = 0;

private:

  FabricCore::DFGBinding m_binding;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Binding_QUndo__
