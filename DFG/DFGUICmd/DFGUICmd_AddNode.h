// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddNode__
#define __UI_DFG_DFGUICmd_AddNode__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddNode : public DFGUICmd_Exec
{
public:

  DFGUICmd_AddNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString primaryArg,
    QPointF pos
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_primaryArg( primaryArg.trimmed() )
    , m_pos( pos )
    {}

  QString getActualNodeName() const
  {
    assert( wasInvoked() );
    return m_actualNodeName;
  }

protected:

  QString getPrimaryArg()
    { return m_primaryArg; }
  QPointF getPos()
    { return m_pos; }
  
  virtual void invoke( unsigned &coreUndoCount );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount )
  {
    // [pzion 20151216] This should be abstract, but Shiboken can't deal
    // with abstract classes
    return FTL::CStrRef();
  }

private:

  QString m_primaryArg;
  QPointF m_pos;
  
  QString m_actualNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddNode__
