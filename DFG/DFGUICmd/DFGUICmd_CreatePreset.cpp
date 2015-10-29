//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_CreatePreset.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_CreatePreset::appendDesc( std::string &desc )
{
  desc += FTL_STR("Create preset '");
  desc += m_presetName;
  desc += FTL_STR("' from node ");
  appendDesc_NodeName( m_nodeName, desc );
}

void DFGUICmd_CreatePreset::invoke( unsigned &coreUndoCount )
{
  FabricCore::DFGExec &exec = getExec();

  FabricCore::DFGExec subExec = exec.getSubExec( m_nodeName.c_str() );

  subExec.maybeSplitFromPreset();
  ++coreUndoCount;

  subExec.setTitle( m_presetName.c_str() );
  ++coreUndoCount;

  FTL::CStrRef uiNodeColor =
    exec.getNodeMetadata( m_nodeName.c_str(), "uiNodeColor" );
  if ( !uiNodeColor.empty() )
  {
    subExec.setMetadata( "uiNodeColor", uiNodeColor.c_str() );
    ++coreUndoCount;
  }
  FTL::CStrRef uiHeaderColor =
    exec.getNodeMetadata( m_nodeName.c_str(), "uiHeaderColor" );
  if ( !uiHeaderColor.empty() )
  {
    subExec.setMetadata( "uiHeaderColor", uiHeaderColor.c_str() );
    ++coreUndoCount;
  }
  FTL::CStrRef uiTextColor =
    exec.getNodeMetadata( m_nodeName.c_str(), "uiTextColor" );
  if ( !uiTextColor.empty() )
  {
    subExec.setMetadata( "uiTextColor", uiTextColor.c_str() );
    ++coreUndoCount;
  }
  FTL::CStrRef uiTooltip =
    exec.getNodeMetadata( m_nodeName.c_str(), "uiTooltip" );
  if ( !uiTooltip.empty() )
  {
    subExec.setMetadata( "uiTooltip", uiTooltip.c_str() );
    ++coreUndoCount;
  }
  FTL::CStrRef uiDocUrl =
    exec.getNodeMetadata( m_nodeName.c_str(), "uiDocUrl" );
  if ( !uiDocUrl.empty() )
  {
    subExec.setMetadata( "uiDocUrl", uiDocUrl.c_str() );
    ++coreUndoCount;
  }
  FTL::CStrRef uiAlwaysShowDaisyChainPorts =
    exec.getNodeMetadata( m_nodeName.c_str(), "uiAlwaysShowDaisyChainPorts" );
  if ( !uiAlwaysShowDaisyChainPorts.empty() )
  {
    subExec.setMetadata(
      "uiAlwaysShowDaisyChainPorts",
      uiAlwaysShowDaisyChainPorts.c_str(),
      true,
      true
      );
    ++coreUndoCount;
  }

  // copy all defaults
  unsigned execPortCount = subExec.getExecPortCount();
  for ( unsigned i = 0; i < execPortCount; ++i )
  {
    std::string pinPath = m_nodeName;
    pinPath += ".";
    pinPath += subExec.getExecPortName(i);

    FTL::StrRef rType = exec.getNodePortResolvedType(pinPath.c_str());
    if(rType.size() == 0 || rType.find('$') != rType.end())
      rType = subExec.getExecPortResolvedType(i);
    if(rType.size() == 0 || rType.find('$') != rType.end())
      rType = subExec.getExecPortTypeSpec(i);
    if(rType.size() == 0 || rType.find('$') != rType.end())
      continue;

    if ( FabricCore::RTVal defaultValueRTVal =
      exec.getInstPortResolvedDefaultValue( pinPath.c_str(), rType.data() ) )
    {
      subExec.setPortDefaultValue(
        subExec.getExecPortName(i),
        defaultValueRTVal
        );
      ++coreUndoCount;
    }
  }

  FabricCore::DFGHost host = getHost();
  m_pathname =
    DFGUICmdHandler::NewPresetPathname( host, m_presetDirPath, m_presetName );

  // Important: set the preset file association BEFORE getting the JSON,
  // as with this association the Core will generate a presetGUID.
  subExec.setImportPathname( m_pathname.c_str() );
  ++coreUndoCount;

  subExec.attachPresetFile(
    m_presetDirPath.c_str(),
    m_presetName.c_str(),
    true // replaceExisting
    );
  ++coreUndoCount;

  FabricCore::String presetJSON = subExec.exportJSON();
  FILE *file = fopen( m_pathname.c_str(), "wb" );
  if ( file )
  {
    fwrite( presetJSON.getCString(), presetJSON.getSize(), 1, file );
    fclose( file );
  }
  else m_pathname = std::string();
}

FABRIC_UI_DFG_NAMESPACE_END
