// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_DismissLoadDiags.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_DismissLoadDiags::appendDesc( QString &desc )
{
  desc += "Dismiss load diagnostic";
  if ( m_diagIndices.size() != 1 )
    desc += 's';
  desc += " [";
  for ( int i = 0; i < m_diagIndices.size(); ++i )
  {
    if ( i != 0 )
      desc += ',';
    char buffer[16];
    sprintf( buffer, "%d", m_diagIndices[i] );
    desc += buffer;
  }
  desc += ']';
}

void DFGUICmd_DismissLoadDiags::invoke( unsigned &coreUndoCount )
{
  Perform(
    getBinding(),
    m_diagIndices,
    coreUndoCount
    );
}

void DFGUICmd_DismissLoadDiags::Perform(
  FabricCore::DFGBinding &binding,
  QList<int> diagIndices,
  unsigned &coreUndoCount
  )
{
  for ( int i = 0; i < diagIndices.size(); ++i )
  {
    try
    {
      binding.dismissLoadDiag( diagIndices[i] );
      ++coreUndoCount;
    }
    catch ( FTL::JSONException e )
    {
      printf("DFGUICmd_DismissLoadDiags: JSON exception: '%s'\n", e.getDescCStr());
    }
  }
}

FABRIC_UI_DFG_NAMESPACE_END
