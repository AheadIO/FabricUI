//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_EditNode.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_EditNode::appendDesc( QString &desc )
{
  desc += "EditNode ";
  appendDesc_NodeName( m_oldNodeName, desc );
  if ( m_actualNewNodeName != m_oldNodeName )
  {
    desc += " (renamed to ";
    appendDesc_NodeName( m_actualNewNodeName, desc );
    desc += ")";
  }
}

void DFGUICmd_EditNode::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_oldNodeName.toUtf8().constData(),
    m_desiredNewNodeName.toUtf8().constData(),
    m_nodeMetadata.toUtf8().constData(),
    m_execMetadata.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_EditNode::invoke(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef desiredNewNodeName,
  FTL::CStrRef nodeMetadata,
  FTL::CStrRef execMetadata,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGExec &exec = getExec();

  FTL::CStrRef actualNewNodeName =
    exec.renameNode(
      oldNodeName.c_str(),
      desiredNewNodeName.c_str()
      );
  m_actualNewNodeName =
    QString::fromUtf8( actualNewNodeName.data(), actualNewNodeName.size() );
  ++coreUndoCount;

  if ( !nodeMetadata.empty() )
  {
    try
    {
      FTL::JSONStrWithLoc swl( nodeMetadata );
      FTL::OwnedPtr<FTL::JSONObject> jo(
        FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
        );

      for ( FTL::JSONObject::const_iterator it = jo->begin();
        it != jo->end(); ++it )
      {
        FTL::CStrRef key = it->first;
        FTL::CStrRef value = it->second->getStringValue();
        exec.setNodeMetadata(
          actualNewNodeName.c_str(),
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

  if ( !execMetadata.empty() )
  {
    FabricCore::DFGExec subExec =
      exec.getSubExec( actualNewNodeName.c_str() );

    try
    {
      FTL::JSONStrWithLoc swl( execMetadata );
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
