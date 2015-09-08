// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ReorderPorts__
#define __UI_DFG_DFGUICmd_ReorderPorts__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ReorderPorts
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_ReorderPorts(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    const std::vector<unsigned int> & indices
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_indices( indices )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("ReorderPorts"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::vector<unsigned int> m_indices;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ReorderPorts__
