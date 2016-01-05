// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_InstPreset__
#define __UI_DFG_DFGUICmd_InstPreset__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_InstPreset
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_InstPreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString presetPath,
    QPointF pos
    )
    : DFGUICmd_AddNode(
      binding,
      execPath,
      exec,
      presetPath,
      pos
      )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("InstPreset"); }

protected:

  QString getPresetPath()
    { return getPrimaryArg(); }
  
  virtual void appendDesc( QString &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );

  FTL::CStrRef invokeAdd(
    FTL::CStrRef presetPath,
    unsigned &coreUndoCount
    );
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_InstPreset__
