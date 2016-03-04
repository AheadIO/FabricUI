// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetNodeComment__
#define __UI_DFG_DFGUICmd_SetNodeComment__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetNodeComment
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetNodeComment(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString comment
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_nodeName( nodeName )
    , m_comment( comment )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetNodeComment"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::CStrRef nodeName,
    FTL::CStrRef comment,
    unsigned &coreUndoCount
    );

private:

  QString m_nodeName;
  QString m_comment;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetNodeComment__
