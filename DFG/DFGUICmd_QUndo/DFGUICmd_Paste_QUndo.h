// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Paste_QUndo__
#define __UI_DFG_DFGUICmd_Paste_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>
#include <QtCore/QPoint>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Paste_QUndo
  : public DFGUICmd_QUndo
{
public:

  DFGUICmd_Paste_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::StrRef json,
    QPointF cursorPos
    )
    : DFGUICmd_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_json( json )
    , m_cursorPos( cursorPos )
    {}

  std::vector<std::string> const &getNodeNames()
    { return m_nodeNames; }

protected:
  
  virtual void invoke(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    m_nodeNames =
      DFGUIPerform_Paste(
        binding,
        execPath,
        exec,
        m_json,
        m_cursorPos,
        coreUndoCount
        );
  }

private:

  std::string m_json;
  QPointF m_cursorPos;

  std::vector<std::string> m_nodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Paste_QUndo__
