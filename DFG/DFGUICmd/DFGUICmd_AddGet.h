// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddGet__
#define __UI_DFG_DFGUICmd_AddGet__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddGet
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_AddGet(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString varPath,
    QPointF pos
    )
    : DFGUICmd_AddNode(
      binding,
      execPath,
      exec,
      desiredNodeName,
      pos
      )
    , m_varPath( varPath.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddGet"); }

protected:

  QString getDesiredNodeName()
    { return getPrimaryArg(); }
  
  virtual void appendDesc( QString &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );

  FTL::CStrRef invokeAdd(
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef varPath,
    unsigned &coreUndoCount
    );

private:

  QString m_varPath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddGet__
