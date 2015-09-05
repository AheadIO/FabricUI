// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddBackDrop__
#define __UI_DFG_DFGUICmd_AddBackDrop__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddBackDrop
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_AddBackDrop(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef title,
    QPointF pos
    )
    : DFGUICmd_AddNode(
      binding,
      execPath,
      exec,
      title,
      pos
      )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddBackDrop"); }

protected:

  FTL::CStrRef getTitle()
    { return getPrimaryArg(); }
  
  virtual void appendDesc( std::string &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddBackDrop__
