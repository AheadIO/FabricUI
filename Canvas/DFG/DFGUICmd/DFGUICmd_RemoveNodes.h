// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RemoveNodes__
#define __UI_DFG_DFGUICmd_RemoveNodes__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RemoveNodes
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_RemoveNodes(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::StrRef> nodeNames
    )
    : DFGUICmd_Exec( binding, execPath, exec )
  {
    m_nodeNames.insert(
      m_nodeNames.end(), nodeNames.begin(), nodeNames.end()
      );
  }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("RemoveNodes"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::vector<std::string> m_nodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemoveNodes__
