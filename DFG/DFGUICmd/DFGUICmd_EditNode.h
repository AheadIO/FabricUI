// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_EditNode__
#define __UI_DFG_DFGUICmd_EditNode__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_EditNode
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_EditNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldNodeName,
    QString desiredNewNodeName,
    QString nodeMetadata,
    QString execMetadata
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_oldNodeName( oldNodeName )
    , m_desiredNewNodeName( desiredNewNodeName )
    , m_nodeMetadata( nodeMetadata )
    , m_execMetadata( execMetadata )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("EditNode"); }

  QString getActualNewNodeName() const
    { return m_actualNewNodeName; }
  
protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::CStrRef oldNodeName,
    FTL::CStrRef desiredNewNodeName,
    FTL::CStrRef nodeMetadata,
    FTL::CStrRef execMetadata,
    unsigned &coreUndoCount
    );

private:

  QString m_oldNodeName;
  QString m_desiredNewNodeName;
  QString m_nodeMetadata;
  QString m_execMetadata;

  QString m_actualNewNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_EditNode__
