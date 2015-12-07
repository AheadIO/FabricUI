// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Paste__
#define __UI_DFG_DFGUICmd_Paste__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <QtCore/QPoint>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Paste
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_Paste(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef json,
    QPointF cursorPos
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_json( json )
    , m_cursorPos( cursorPos )
  {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("Paste"); }

  FTL::ArrayRef<std::string> getPastedNodeNames()
  {
    assert( wasInvoked() );
    return m_pastedNodeNames;
  }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_json;
  QPointF m_cursorPos;

  std::vector<std::string> m_pastedNodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Paste__
