// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetNodeComment__
#define __UI_DFG_DFGUICmd_SetNodeComment__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetNodeComment
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetNodeComment(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef nodeName,
    FTL::StrRef comment
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_nodeName( nodeName )
    , m_comment( comment )
    {}

  static FTL::CStrRef CmdName()
    { return FTL_STR("dfgSetNodeComment"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_nodeName;
  std::string m_comment;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetNodeComment__
