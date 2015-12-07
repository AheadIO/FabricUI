// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ResizeBackDrop__
#define __UI_DFG_DFGUICmd_ResizeBackDrop__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ResizeBackDrop
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_ResizeBackDrop(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef backDropName,
    QPointF newTopLeftPos,
    QSizeF newSize
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_backDropName( backDropName )
    , m_newTopLeftPos( newTopLeftPos )
    , m_newSize( newSize )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("ResizeBackDrop"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_backDropName;
  QPointF m_newTopLeftPos;
  QSizeF m_newSize;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ResizeBackDrop__
