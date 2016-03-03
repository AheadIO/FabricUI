// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddNode__
#define __UI_DFG_DFGUICmd_AddNode__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddNode : public DFGUICmd_Exec
{
public:

  DFGUICmd_AddNode(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef primaryArg,
    QPointF pos
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_primaryArg( primaryArg )
    , m_pos( pos )
    {}

  FTL::CStrRef getActualNodeName() const
  {
    assert( wasInvoked() );
    return m_actualNodeName;
  }

protected:

  FTL::CStrRef getPrimaryArg()
    { return m_primaryArg; }
  QPointF getPos()
    { return m_pos; }
  
  virtual void invoke( unsigned &coreUndoCount );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount ) = 0;

private:

  std::string m_primaryArg;
  QPointF m_pos;
  
  std::string m_actualNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddNode__
