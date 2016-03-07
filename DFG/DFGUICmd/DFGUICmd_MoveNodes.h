// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_MoveNodes__
#define __UI_DFG_DFGUICmd_MoveNodes__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_MoveNodes
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_MoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames,
    QList<QPointF> newTopLeftPoss
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_nodeNames( nodeNames )
    , m_newTopLeftPoss( newTopLeftPoss )
  {
  }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("MoveNodes"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  QList<QString> m_nodeNames;
  QList<QPointF> m_newTopLeftPoss;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_MoveNodes__
