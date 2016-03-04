// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ImplodeNodes__
#define __UI_DFG_DFGUICmd_ImplodeNodes__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ImplodeNodes
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_ImplodeNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QList<QString> nodeNames,
    QString desiredImplodedNodeName
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_nodeNames( nodeNames )
    , m_desiredImplodedNodeName( desiredImplodedNodeName )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("ImplodeNodes"); }

  QString getActualImplodedNodeName()
  {
    assert( wasInvoked() );
    return m_actualImplodedNodeName;
  }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  FTL::CStrRef invoke(
    FTL::ArrayRef<FTL::CStrRef> nodeNames,
    FTL::CStrRef desiredImplodedNodeName,
    unsigned &coreUndoCount
    );

private:

  QList<QString> m_nodeNames;
  QString m_desiredImplodedNodeName;

  QString m_actualImplodedNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ImplodeNodes__
