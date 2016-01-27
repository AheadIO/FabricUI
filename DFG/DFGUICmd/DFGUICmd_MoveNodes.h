// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_MoveNodes__
#define __UI_DFG_DFGUICmd_MoveNodes__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_MoveNodes
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_MoveNodes(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::StrRef> nodeNames,
    FTL::ArrayRef<QPointF> newTopLeftPoss
    )
    : DFGUICmd_Exec( binding, execPath, exec )
  {
    m_nodeNames.insert(
      m_nodeNames.begin(),
      nodeNames.begin(),
      nodeNames.end()
      );
    m_newTopLeftPoss.insert(
      m_newTopLeftPoss.begin(),
      newTopLeftPoss.begin(),
      newTopLeftPoss.end()
      );
  }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("MoveNodes"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::vector<std::string> m_nodeNames;
  std::vector<QPointF> m_newTopLeftPoss;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_MoveNodes__
