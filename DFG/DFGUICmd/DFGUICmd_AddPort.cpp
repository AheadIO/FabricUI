// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddPort.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddPort::appendDesc( std::string &desc )
{
  desc += FTL_STR("Add ");
  appendDesc_PortPath( getActualPortName(), desc );
}

void DFGUICmd_AddPort::invoke( unsigned &coreUndoCount )
{
  m_actualPortName =
    Perform(
      getBinding(),
      getExecPath(),
      getExec(),
      m_desiredPortName,
      m_portType,
      m_typeSpec,
      m_portToConnectWith,
      coreUndoCount
      );
}

FTL::CStrRef DFGUICmd_AddPort::Perform(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredPortName,
  FabricCore::DFGPortType portType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef portName =
    exec.addExecPort(
      desiredPortName.c_str(),
      portType,
      typeSpec.c_str()
      );
  ++coreUndoCount;

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

        char const *metadatasToCopy[3] =
        {
          "uiRange",
          "uiCombo",
          "uiHidden"
        };

        for ( unsigned i = 0; i < 3; ++i )
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

    if ( portType != FabricCore::DFGPortType_Out
      && portToConnectNodePortType != FabricCore::DFGPortType_Out )
    {
      exec.connectTo( portName.c_str(), portToConnect.c_str() );
      ++coreUndoCount;
    }
    if ( portType != FabricCore::DFGPortType_In )
    {
      exec.connectTo( portToConnect.c_str(), portName.c_str() );
      ++coreUndoCount;
    }
  }
  else if ( execPath.empty()
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
      portName.c_str(),
      argValue,
      true
      );
    ++coreUndoCount;
  }

  return portName;
}

FABRIC_UI_DFG_NAMESPACE_END
