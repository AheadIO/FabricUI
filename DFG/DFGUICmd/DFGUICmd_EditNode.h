// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_EditNode__
#define __UI_DFG_DFGUICmd_EditNode__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_EditNode
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_EditNode(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef oldNodeName,
    FTL::StrRef desiredNewNodeName,
    FTL::StrRef nodeMetadata,
    FTL::StrRef execMetadata
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_oldNodeName( oldNodeName )
    , m_desiredNewNodeName( desiredNewNodeName )
    , m_nodeMetadata( nodeMetadata )
    , m_execMetadata( execMetadata )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("EditNode"); }

  FTL::CStrRef getActualNewNodeName() const
    { return m_actualNewNodeName; }
  
protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_oldNodeName;
  std::string m_desiredNewNodeName;
  std::string m_nodeMetadata;
  std::string m_execMetadata;

  std::string m_actualNewNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_EditNode__
