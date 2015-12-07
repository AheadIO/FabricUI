//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUICmd_ExplodeNode__
#define __UI_DFG_DFGUICmd_ExplodeNode__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ExplodeNode
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_ExplodeNode(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef nodeName
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_nodeName( nodeName )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("ExplodeNode"); }

  FTL::ArrayRef<std::string> getExplodedNodeNames()
  {
    assert( wasInvoked() );
    return m_explodedNodeNames;
  }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  std::vector<std::string> Perform(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef nodeName,
    unsigned &coreUndoCount
    );

private:

  std::string m_nodeName;

  std::vector<std::string> m_explodedNodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ExplodeNode__
