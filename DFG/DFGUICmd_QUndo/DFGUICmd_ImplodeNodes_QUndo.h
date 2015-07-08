// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ImplodeNodes_QUndo__
#define __UI_DFG_DFGUICmd_ImplodeNodes_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ImplodeNodes_QUndo
  : public DFGUICmd_QUndo
{
public:

  DFGUICmd_ImplodeNodes_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::ArrayRef<FTL::CStrRef> nodeNames
    )
    : DFGUICmd_QUndo( desc, binding, execPath, exec )
    , m_desiredNodeName( desiredNodeName )
  {
    m_nodeNames.reserve( nodeNames.size() );
    for ( FTL::ArrayRef<FTL::CStrRef>::IT it = nodeNames.begin();
      it != nodeNames.end(); ++it )
    {
      m_nodeNames.push_back( *it );
    }
  }

  FTL::CStrRef getImplodedNodeName()
    { return m_implodedNodeName; }

protected:
  
  virtual void invoke(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    m_implodedNodeName =
      DFGUIPerform_ImplodeNodes(
        binding,
        execPath,
        exec,
        m_desiredNodeName,
        m_nodeNames,
        coreUndoCount
        );
  }

private:

  std::string m_desiredNodeName;
  std::vector<std::string> m_nodeNames;

  std::string m_implodedNodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ImplodeNodes_QUndo__
