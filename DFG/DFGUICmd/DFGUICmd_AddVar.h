// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddVar__
#define __UI_DFG_DFGUICmd_AddVar__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddVar
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_AddVar(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef desiredNodeName,
    FTL::StrRef dataType,
    FTL::StrRef extDep,
    QPointF pos
    )
    : DFGUICmd_AddNode(
      binding,
      execPath,
      exec,
      desiredNodeName,
      pos
      )
    , m_dataType( dataType )
    , m_extDep( extDep )
    {}

  static FTL::CStrRef CmdName()
    { return FTL_STR("dfgAddVar"); }

protected:

  FTL::CStrRef getDesiredNodeName()
    { return getPrimaryArg(); }
  
  virtual void appendDesc( std::string &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );

private:

  std::string m_dataType;
  std::string m_extDep;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddVar__
