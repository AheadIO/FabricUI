//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetExtDeps.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetExtDeps::appendDesc( QString &desc )
{
  desc += "SetExtDeps";
  QString execPath = getExecPath();
  if ( !execPath.isEmpty() )
  {
    desc += ' ';
    desc += execPath;
  }
}

void DFGUICmd_SetExtDeps::invoke( unsigned &coreUndoCount )
{
  QList<QByteArray> extDepBAs;
#if QT_VERSION >= 0x040800
  extDepBAs.reserve( m_extDeps.size() );
#endif
  foreach ( QString extDep, m_extDeps )
    extDepBAs.append( extDep.toUtf8() );

  char const **extDepCStrs =
    static_cast<char const **>(
      alloca( m_extDeps.size() * sizeof( char const * ) )
      );
  for ( int i = 0; i < m_extDeps.size(); ++i )
    extDepCStrs[i] = extDepBAs[i].constData();

  invoke(
    FTL::ArrayRef<char const *>( extDepCStrs, m_extDeps.size() ),
    coreUndoCount
    );
}
  
void DFGUICmd_SetExtDeps::invoke(
  FTL::ArrayRef<char const *> extDepCStrs,
  unsigned &coreUndoCount
  )
{
  getExec().setExtDeps( extDepCStrs.size(), extDepCStrs.data() );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
