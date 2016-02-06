// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_EditPort.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_EditPort::appendDesc( std::string &desc )
{
  desc += FTL_STR("Edit ");
  appendDesc_PortPath( m_oldPortName, desc );
  if ( m_actualNewPortName != m_oldPortName )
  {
    desc += FTL_STR("(renamed to ");
    appendDesc_PortPath( m_actualNewPortName, desc );
    desc += ')';
  }
}

void DFGUICmd_EditPort::invoke( unsigned &coreUndoCount )
{
  m_actualNewPortName =
    Perform(
      getBinding(),
      getExecPath(),
      getExec(),
      m_oldPortName,
      m_desiredNewPortName,
      m_typeSpec,
      m_extDep,
      m_uiMetadata,
      coreUndoCount
      );
}

FTL::CStrRef DFGUICmd_EditPort::Perform(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef oldPortName,
  FTL::CStrRef desiredNewPortName,
  FTL::CStrRef typeSpec,
  FTL::CStrRef extDep,
  FTL::CStrRef uiMetadata,
  unsigned &coreUndoCount
  )
{
  if ( !extDep.empty() )
  {
    exec.addExtDep( extDep.c_str() );
    ++coreUndoCount;
  }

  FTL::CStrRef newPortName;
  if ( !desiredNewPortName.empty()
    && desiredNewPortName != oldPortName )
  {
    newPortName = exec.renameExecPort(
      oldPortName.c_str(),
      desiredNewPortName.c_str()
      );
    ++coreUndoCount;
  }
  else
    newPortName = oldPortName;

  // Only set type & value if the type is different (else we loose the value even if the type didn't change!)
  FTL::CStrRef prevType = exec.getExecPortTypeSpec( newPortName.c_str() );
  if ( !typeSpec.empty() && prevType != typeSpec )
  {
    exec.setExecPortTypeSpec(
      newPortName.c_str(),
      typeSpec.c_str()
      );
    ++coreUndoCount;

    if ( execPath.empty()
      && !typeSpec.empty()
      && typeSpec.find('$') == typeSpec.end() )
    {
      FabricCore::DFGHost host = binding.getHost();
      FabricCore::Context context = host.getContext();



      FabricCore::RTVal argValue =
        FabricCore::RTVal::Construct(
          context,
          typeSpec.c_str(),
          0,
          0
          );
      binding.setArgValue(
        newPortName.c_str(),
        argValue,
        true
        );
      ++coreUndoCount;
    }
  }

  if ( !uiMetadata.empty() )
  {
    try
    {
      FTL::JSONStrWithLoc swl( uiMetadata );
      FTL::OwnedPtr<FTL::JSONObject> jo(
        FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
        );

      for ( FTL::JSONObject::const_iterator it = jo->begin();
        it != jo->end(); ++it )
      {
        FTL::CStrRef key = it->first;
        FTL::CStrRef value = it->second->getStringValue();
        exec.setExecPortMetadata(
          newPortName.c_str(),
          key.c_str(),
          !value.empty()? value.c_str(): NULL,
          true
          );
        ++coreUndoCount;
      }
    }
    catch ( FTL::JSONException e )
    {
      printf("DFGUICmd_EditPort: JSON exception: '%s'\n", e.getDescCStr());
    }
  }

  return newPortName;
}

FABRIC_UI_DFG_NAMESPACE_END
