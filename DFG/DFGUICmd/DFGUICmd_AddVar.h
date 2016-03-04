// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddVar__
#define __UI_DFG_DFGUICmd_AddVar__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddVar
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_AddVar(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString dataType,
    QString extDep,
    QPointF pos
    )
    : DFGUICmd_AddNode(
      binding,
      execPath,
      exec,
      desiredNodeName,
      pos
      )
    , m_dataType( dataType.trimmed() )
    , m_extDep( extDep.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddVar"); }

protected:

  QString getDesiredNodeName()
    { return getPrimaryArg(); }
  
  virtual void appendDesc( QString &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );

  FTL::CStrRef invokeAdd(
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef dataType,
    FTL::CStrRef extDep,
    unsigned &coreUndoCount
    );

private:

  QString m_dataType;
  QString m_extDep;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddVar__
