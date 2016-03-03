//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetExtDeps.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetExtDeps::appendDesc( std::string &desc )
{
  desc += FTL_STR("SetExtDeps");
  FTL::StrRef execPath = getExecPath();
  if ( !execPath.empty() )
  {
    desc += ' ';
    desc += execPath;
  }
}

void DFGUICmd_SetExtDeps::invoke( unsigned &coreUndoCount )
{
  char const **nameAndVerCStrs =
    static_cast<char const **>(
      alloca( m_extDeps.size() * sizeof( char const * ) )
      );

  for ( size_t i = 0; i < m_extDeps.size(); ++i )
    nameAndVerCStrs[i] = m_extDeps[i].c_str();
  
  getExec().setExtDeps( m_extDeps.size(), nameAndVerCStrs );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
