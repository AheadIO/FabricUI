// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddGraph__
#define __UI_DFG_DFGUICmd_AddGraph__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddGraph
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_AddGraph(
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
    { return DFG_CMD_NAME("AddGraph"); }

protected:

  FTL::CStrRef getDesiredNodeName()
    { return getPrimaryArg(); }
  
  virtual void appendDesc( std::string &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddGraph__
