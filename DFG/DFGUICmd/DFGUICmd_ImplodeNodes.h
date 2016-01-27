// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ImplodeNodes__
#define __UI_DFG_DFGUICmd_ImplodeNodes__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ImplodeNodes
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_ImplodeNodes(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::StrRef> nodeNames,
    FTL::StrRef desiredImplodedNodeName
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_desiredImplodedNodeName( desiredImplodedNodeName )
  {
    m_nodeNames.insert(
      m_nodeNames.end(),
      nodeNames.begin(),
      nodeNames.end()
      );
  }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("ImplodeNodes"); }

  FTL::CStrRef getActualImplodedNodeName()
  {
    assert( wasInvoked() );
    return m_actualImplodedNodeName;
  }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  static std::string Perform(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::ArrayRef<std::string> nodeNames,
    FTL::CStrRef desiredImplodedNodeName,
    unsigned &coreUndoCount
    );

private:

  std::vector<std::string> m_nodeNames;
  std::string m_desiredImplodedNodeName;

  std::string m_actualImplodedNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ImplodeNodes__
