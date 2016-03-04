// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RemoveNodes__
#define __UI_DFG_DFGUICmd_RemoveNodes__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RemoveNodes
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_RemoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_nodeNames( nodeNames )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("RemoveNodes"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::ArrayRef<FTL::CStrRef> nodeNames,
    unsigned &coreUndoCount
    );

private:

  QStringList m_nodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemoveNodes__
