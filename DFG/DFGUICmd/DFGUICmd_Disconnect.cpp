//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Disconnect.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Disconnect::appendDesc( QString &desc )
{
  desc += "Disconnect ";
  appendDesc_Path( "\"" + m_srcPaths.join("|") + "\"", desc );
  desc += " from ";
  appendDesc_Path( "\"" + m_dstPaths.join("|") + "\"", desc );
}

void DFGUICmd_Disconnect::invoke( unsigned &coreUndoCount )
{
  QList<QByteArray> srcPathBAs;
  srcPathBAs.reserve( m_srcPaths.size() );
  foreach ( QString nodeName, m_srcPaths )
    srcPathBAs.push_back( nodeName.toUtf8() );

  std::vector<FTL::CStrRef> srcPaths;
  srcPaths.reserve( m_srcPaths.size() );
  foreach ( QByteArray srcPathBA, srcPathBAs )
    srcPaths.push_back( srcPathBA.constData() );

  QList<QByteArray> dstPathBAs;
  dstPathBAs.reserve( m_dstPaths.size() );
  foreach ( QString nodeName, m_dstPaths )
    dstPathBAs.push_back( nodeName.toUtf8() );

  std::vector<FTL::CStrRef> dstPaths;
  dstPaths.reserve( m_dstPaths.size() );
  foreach ( QByteArray dstPathBA, dstPathBAs )
    dstPaths.push_back( dstPathBA.constData() );

  invoke(
    srcPaths,
    dstPaths,
    coreUndoCount
    );
}

void DFGUICmd_Disconnect::invoke(
  FTL::ArrayRef<FTL::CStrRef> srcPaths,
  FTL::ArrayRef<FTL::CStrRef> dstPaths,
  unsigned &coreUndoCount
  )
{
  size_t num = ( srcPaths.size() <= dstPaths.size() ? srcPaths.size() : dstPaths.size() );
  for ( size_t i = 0; i < num; ++i )
  {
    getExec().disconnectFrom(
      srcPaths[i].c_str(),
      dstPaths[i].c_str()
      );
    ++coreUndoCount;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
