// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddFunc__
#define __UI_DFG_DFGUICmd_AddFunc__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddFunc
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_AddFunc(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QString initialCode,
    QPointF pos
    )
    : DFGUICmd_AddNode(
      binding,
      execPath,
      exec,
      title,
      pos
      )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddFunc"); }

protected:

  QString getDesiredNodeName()
    { return getPrimaryArg(); }
  
  virtual void appendDesc( QString &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );

  FTL::CStrRef invokeAdd(
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef initialCode,
    unsigned &coreUndoCount
    );

private:

  QString m_initialCode;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddFunc__
