// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

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
    FTL::StrRef desiredImplodedNodeName,
    FTL::ArrayRef<FTL::StrRef> nodeNames
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
    { return FTL_STR("dfgImplodeNodes"); }

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
    FTL::CStrRef desiredImplodedNodeName,
    FTL::ArrayRef<std::string> nodeNames,
    unsigned &coreUndoCount
    );

private:

  std::vector<std::string> m_nodeNames;
  std::string m_desiredImplodedNodeName;

  std::string m_actualImplodedNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ImplodeNodes__
