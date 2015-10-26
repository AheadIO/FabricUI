//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_EditNode.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_EditNode::appendDesc( std::string &desc )
{
  desc += FTL_STR("EditNode ");
  appendDesc_NodeName( m_oldNodeName, desc );
  if ( m_actualNewNodeName != m_oldNodeName )
  {
    desc += FTL_STR(" (renamed to ");
    appendDesc_NodeName( m_actualNewNodeName, desc );
    desc += FTL_STR(")");
  }
}

void DFGUICmd_EditNode::invoke( unsigned &coreUndoCount )
{
  FabricCore::DFGExec &exec = getExec();

  m_actualNewNodeName = exec.renameNode(
    m_oldNodeName.c_str(),
    m_desiredNewNodeName.c_str()
    );
  ++coreUndoCount;

  if ( !m_nodeMetadata.empty() )
  {
    try
    {
      FTL::JSONStrWithLoc swl( m_nodeMetadata );
      FTL::OwnedPtr<FTL::JSONObject> jo(
        FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
        );

      for ( FTL::JSONObject::const_iterator it = jo->begin();
        it != jo->end(); ++it )
      {
        FTL::CStrRef key = it->first;
        FTL::CStrRef value = it->second->getStringValue();
        exec.setNodeMetadata(
          m_actualNewNodeName.c_str(),
          key.c_str(),
          !value.empty()? value.c_str(): NULL,
          true
          );
        ++coreUndoCount;
      }
    }
    catch ( FTL::JSONException e )
    {
      printf("DFGUICmd_EditNode: JSON exception: '%s'\n", e.getDescCStr());
    }
  }

  if ( !m_execMetadata.empty() )
  {
    FabricCore::DFGExec subExec =
      exec.getSubExec( m_actualNewNodeName.c_str() );

    try
    {
      FTL::JSONStrWithLoc swl( m_execMetadata );
      FTL::OwnedPtr<FTL::JSONObject> jo(
        FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
        );

      for ( FTL::JSONObject::const_iterator it = jo->begin();
        it != jo->end(); ++it )
      {
        FTL::CStrRef key = it->first;
        FTL::CStrRef value = it->second->getStringValue();
        subExec.setMetadata(
          key.c_str(),
          !value.empty()? value.c_str(): NULL,
          true
          );
        ++coreUndoCount;
      }
    }
    catch ( FTL::JSONException e )
    {
      printf("DFGUICmd_EditNode: JSON exception: '%s'\n", e.getDescCStr());
    }
  }
}

FABRIC_UI_DFG_NAMESPACE_END
