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
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString presetDirPath,
    QString presetName
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_nodeName( nodeName.trimmed() )
    , m_presetDirPath( presetDirPath.trimmed() )
    , m_presetName( presetName.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("CreatePreset"); }

  QString getPathname() const
    { return m_pathname; }
  
protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  QString invoke(
    FTL::CStrRef nodeName,
    FTL::CStrRef presetDirPath,
    FTL::CStrRef presetName,
    unsigned &coreUndoCount
    );

private:

  QString m_nodeName;
  QString m_presetDirPath;
  QString m_presetName;

  QString m_pathname;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_CreatePreset__
