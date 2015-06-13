// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RemoveNodes_QUndo__
#define __UI_DFG_DFGUICmd_RemoveNodes_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RemoveNodes_QUndo
  : public DFGUICmd_QUndo
{
public:

  DFGUICmd_RemoveNodes_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::StrRef> nodeNames
    )
    : DFGUICmd_QUndo( desc, binding, execPath, exec )
  {
    m_nodeNames.insert(
      m_nodeNames.end(), nodeNames.begin(), nodeNames.end()
      );
  }

protected:
  
  virtual void invoke(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    for ( size_t i = 0; i < m_nodeNames.size(); ++i )
    {
      exec.removeNode( m_nodeNames[i].c_str() );
      ++coreUndoCount;
    }
  }

private:

  std::vector<std::string> m_nodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemoveNodes_QUndo__
