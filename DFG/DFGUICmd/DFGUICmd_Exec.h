// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Exec__
#define __UI_DFG_DFGUICmd_Exec__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Binding.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Exec : public DFGUICmd_Binding
{
public:

  DFGUICmd_Exec(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec
    )
    : DFGUICmd_Binding( binding )
    , m_execPath( execPath )
    , m_exec( exec )
    {}

protected:

  FTL::CStrRef getExecPath()
    { return m_execPath; }
  FabricCore::DFGExec &getExec()
    { return m_exec; }

  void appendDesc_Path(
    FTL::CStrRef path,
    std::string &desc
    )
  {
    desc += '\'';
    desc += m_execPath;
    if ( !m_execPath.empty() )
      desc += '.';
    desc += path;
    desc += '\'';
  }

  void appendDesc_Paths(
    FTL::CStrRef singular,
    FTL::CStrRef plural,
    FTL::ArrayRef<FTL::CStrRef> paths,
    std::string &desc
    )
  {
    if ( paths.size() == 1 )
    {
      desc += singular;
      desc += ' ';
      appendDesc_Path( paths[0], desc );
    }
    else if ( paths.size() > 1 && paths.size() <= 4 )
    {
      desc += plural;
      desc += ' ';
      FTL::ArrayRef<FTL::CStrRef> headPaths = paths.head( 4 );
      FTL::ArrayRef<FTL::CStrRef>::IT itBegin = headPaths.begin();
      FTL::ArrayRef<FTL::CStrRef>::IT itEnd = headPaths.end();
      desc += '[';
      for ( FTL::ArrayRef<FTL::CStrRef>::IT it = itBegin; it != itEnd; ++it )
      {
        if ( it != itBegin )
          desc += ',';
        appendDesc_Path( *it, desc );
      }
      desc += ']';
    }
    else
    {
      char buf[32];
      snprintf( buf, 32, "%u", unsigned( paths.size() ) );

      desc += buf;
      desc += ' ';
      desc += plural;
      desc += FTL_STR(" in ");
      AppendDesc_String( m_execPath, desc );
    }
  }

  void appendDesc_NodeName(
    FTL::CStrRef nodeName,
    std::string &desc
    )
  {
    desc += FTL_STR("node ");
    appendDesc_Path( nodeName, desc );
  }

  void appendDesc_NodeNames(
    FTL::ArrayRef<FTL::CStrRef> nodeNames,
    std::string &desc
    )
  {
    appendDesc_Paths(
      FTL_STR("node"),
      FTL_STR("nodes"),
      nodeNames,
      desc
      );
  }

  void appendDesc_NodeNames(
    FTL::ArrayRef<std::string> nodeNames,
    std::string &desc
    )
  {
    std::vector<FTL::CStrRef> nodeNameCStrRefs;
    nodeNameCStrRefs.insert(
      nodeNameCStrRefs.end(),
      nodeNames.begin(),
      nodeNames.end()
      );
    appendDesc_NodeNames( nodeNameCStrRefs, desc );
  }

  void appendDesc_PortPath(
    FTL::CStrRef refName,
    std::string &desc
    )
  {
    desc += FTL_STR("port ");
    appendDesc_Path( refName, desc );
  }

  std::vector<std::string> adjustNewNodes(
    FabricCore::DFGStringResult const &newNodeNamesJSON,
    QPointF targetPos,
    unsigned &coreUndoCount
    );

  static void MoveNodes(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::ArrayRef<std::string> nodeNames,
    FTL::ArrayRef<QPointF> newTopLeftPoss,
    unsigned &coreUndoCount
    );

  static QPointF GetNodeUIGraphPos( 
    FabricCore::DFGExec &exec,
    FTL::CStrRef nodeName
    );

private:

  std::string m_execPath;
  FabricCore::DFGExec m_exec;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Exec__
