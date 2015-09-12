// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetExtDeps__
#define __UI_DFG_DFGUICmd_SetExtDeps__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetExtDeps
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetExtDeps(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::StrRef> extDeps
    )
    : DFGUICmd_Exec( binding, execPath, exec )
  {
    m_extDeps.reserve( extDeps.size() );
    for ( FTL::ArrayRef<FTL::StrRef>::IT it = extDeps.begin();
      it != extDeps.end(); ++it )
      m_extDeps.push_back( *it );
  }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetExtDeps"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::vector<std::string> m_extDeps;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetExtDeps__
