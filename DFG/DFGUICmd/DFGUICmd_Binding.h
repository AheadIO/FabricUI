// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Binding__
#define __UI_DFG_DFGUICmd_Binding__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Binding : public DFGUICmd
{
public:

  DFGUICmd_Binding(
    FabricCore::DFGBinding const &binding
    )
    : DFGUICmd( binding.getHost() )
    , m_binding( binding )
    {}

protected:

  FabricCore::DFGBinding &getBinding()
    { return m_binding; }

  FabricCore::DFGHost getHost()
    { return m_binding.getHost(); }
  
  void appendDesc_ArgName(
    QString argName,
    QString &desc
    )
  {
    desc += "argument ";
    AppendDesc_String( argName, desc );
  }

private:

  FabricCore::DFGBinding m_binding;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Binding__
