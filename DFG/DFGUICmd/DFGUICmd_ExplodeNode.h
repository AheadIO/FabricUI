//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUICmd_ExplodeNode__
#define __UI_DFG_DFGUICmd_ExplodeNode__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ExplodeNode
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_ExplodeNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_nodeName( nodeName.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("ExplodeNode"); }

  QStringList getExplodedNodeNames()
  {
    assert( wasInvoked() );
    return m_explodedNodeNames;
  }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  QStringList invoke(
    FTL::CStrRef nodeName,
    unsigned &coreUndoCount
    );

private:

  QString m_nodeName;

  QStringList m_explodedNodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ExplodeNode__
