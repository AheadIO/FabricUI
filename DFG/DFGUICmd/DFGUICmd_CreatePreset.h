// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_CreatePreset__
#define __UI_DFG_DFGUICmd_CreatePreset__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_CreatePreset
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_CreatePreset(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef nodeName,
    FTL::StrRef presetDirPath,
    FTL::StrRef presetName
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_nodeName( nodeName )
    , m_presetDirPath( presetDirPath )
    , m_presetName( presetName )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("CreatePreset"); }

  FTL::CStrRef getPathname() const
    { return m_pathname; }
  
protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_nodeName;
  std::string m_presetDirPath;
  std::string m_presetName;

  std::string m_pathname;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_CreatePreset__
