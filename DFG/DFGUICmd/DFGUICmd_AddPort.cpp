// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddPort.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>
#include <Persistence/RTValToJSONEncoder.hpp>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddPort::appendDesc( QString &desc )
{
  desc += "Add ";
  appendDesc_PortPath( getActualPortName(), desc );
}

void DFGUICmd_AddPort::invoke( unsigned &coreUndoCount )
{
  m_actualPortName = QString::fromUtf8(
    invoke(
      getExecPath().toUtf8().constData(),
      m_desiredPortName.toUtf8().constData(),
      m_typeSpec.toUtf8().constData(),
      m_portToConnectWith.toUtf8().constData(),
      m_extDep.toUtf8().constData(),
      m_metaData.toUtf8().constData(),
      coreUndoCount
      ).c_str()
    );
}

FTL::CStrRef DFGUICmd_AddPort::invoke(
  FTL::CStrRef execPath,
  FTL::CStrRef desiredPortName,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect,
  FTL::CStrRef extDep,
  FTL::CStrRef metaData,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGBinding &binding = getBinding();
  FabricCore::DFGExec &exec = getExec();

  if ( !extDep.empty() )
  {
    exec.addExtDep( extDep.c_str() );
    ++coreUndoCount;
  }

  FTL::CStrRef portName =
    exec.addExecPort(
      desiredPortName.c_str(),
      m_portType,
      typeSpec.c_str()
      );
  ++coreUndoCount;

  if ( !metaData.empty() )
  {
    try
    {
      FTL::JSONStrWithLoc swl( metaData );
      FTL::OwnedPtr<FTL::JSONObject> jo(
        FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>()
        );

      FTL::JSONObject::const_iterator it = jo->begin();
      for(;it!=jo->end();it++)
      {
        FTL::CStrRef key = it->first;
        FTL::CStrRef value = it->second->getStringValue();
        exec.setExecPortMetadata(portName.c_str(), key.c_str(), value.c_str(), false);
      }
    }
    catch(FTL::JSONException e)
    {
      printf("DFGUICmd_AddPort: Json exception: '%s'\n", e.getDescCStr());
    }
  }

  // we should always bind an rt val, even if we are
  // going to create a connection lateron.
  // the dccs require this given they listen to the 
  // argTypeChanged notification.
  if ( execPath.empty()
    && !typeSpec.empty()
    && typeSpec.find('$') == typeSpec.end() )
  {
    FabricCore::DFGHost host = getHost();
    FabricCore::Context context = host.getContext();
    FabricCore::RTVal argValue =
      FabricCore::RTVal::Construct(
        context,
        typeSpec.c_str(),
        0,
        0
        );
    binding.setArgValue(
      portName.c_str(),
      argValue,
      true
      );
    ++coreUndoCount;
  }

  if ( !portToConnect.empty() )
  {
    FabricCore::DFGPortType portToConnectNodePortType =
      exec.getNodePortType( portToConnect.c_str() );

    if ( portToConnectNodePortType == FabricCore::DFGPortType_In )
    {
      std::pair<FTL::StrRef, FTL::CStrRef> split = portToConnect.rsplit('.');
      std::string nodeToConnect = split.first;

      if ( !nodeToConnect.empty()
        && exec.getNodeType( nodeToConnect.c_str() )
          == FabricCore::DFGNodeType_Inst )
      {
        FTL::CStrRef nodePortToConnect = split.second;

        FTL::CStrRef resolvedType =
          exec.getNodePortResolvedType( portToConnect.c_str() );
        if ( !resolvedType.empty() )
        {
          FabricCore::RTVal defaultValue =
            exec.getInstPortResolvedDefaultValue(
              portToConnect.c_str(),
              resolvedType.c_str()
              );
          if ( defaultValue.isValid() )
          {
            if ( execPath.empty() )
              binding.setArgValue( portName.c_str(), defaultValue, true );
            else
              exec.setPortDefaultValue( portName.c_str(), defaultValue, true );
            ++coreUndoCount;
          }
        }

        FabricCore::DFGExec subExec =
          exec.getSubExec( nodeToConnect.c_str() );

        char const *metadatasToCopy[5] =
        {
          "uiRange",
          "uiCombo",
          "uiHidden",
          "uiOpaque",
          DFG_METADATA_UIPERSISTVALUE
        };

        for ( unsigned i = 0; i < 5; ++i )
        {
          exec.setExecPortMetadata(
            portName.c_str(),
            metadatasToCopy[i],
            subExec.getExecPortMetadata(
              nodePortToConnect.c_str(),
              metadatasToCopy[i]
              ),
            true
            );
          ++coreUndoCount;
        }
      }
    }

    if ( m_portType != FabricCore::DFGPortType_Out
      && portToConnectNodePortType != FabricCore::DFGPortType_Out )
    {
      exec.connectTo( portName.c_str(), portToConnect.c_str() );
      ++coreUndoCount;
    }
    if ( m_portType != FabricCore::DFGPortType_In )
    {
      exec.connectTo( portToConnect.c_str(), portName.c_str() );
      ++coreUndoCount;
    }
  }

  return portName;
}

FABRIC_UI_DFG_NAMESPACE_END
