// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Exec__
#define __UI_DFG_DFGUICmd_Exec__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Binding.h>

#include <QtCore/QStringList.h>

#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Exec : public DFGUICmd_Binding
{
public:

  DFGUICmd_Exec(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec
    )
    : DFGUICmd_Binding( binding )
    , m_execPath( execPath.trimmed() )
    , m_exec( exec )
    {}

protected:

  QString getExecPath()
    { return m_execPath; }
  FabricCore::DFGExec &getExec()
    { return m_exec; }

  void appendDesc_Path(
    QString path,
    QString &desc
    )
  {
    desc += '\'';
    desc += m_execPath.toUtf8().constData();
    if ( !m_execPath.isEmpty() && !path.isEmpty() )
      desc += '.';
    desc += path.toUtf8().constData();
    desc += '\'';
  }

  void appendDesc_Paths(
    char const *singular,
    char const *plural,
    QStringList paths,
    QString &desc
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
      desc += '[';
      for ( int i = 0; i < paths.size() && i < 4; ++i )
      {
        if ( i != 0 )
          desc += ',';
        appendDesc_Path( paths.at( i ), desc );
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
      desc += " in ";
      AppendDesc_String( m_execPath, desc );
    }
  }

  void appendDesc_NodeName(
    QString nodeName,
    QString &desc
    )
  {
    desc += "node ";
    appendDesc_Path( nodeName, desc );
  }

  void appendDesc_NodeNames(
    QStringList nodeNames,
    QString &desc
    )
  {
    appendDesc_Paths(
      "node",
      "nodes",
      nodeNames,
      desc
      );
  }

  void appendDesc_PortPath(
    QString refName,
    QString &desc
    )
  {
    desc += "port ";
    appendDesc_Path( refName, desc );
  }

  QStringList adjustNewNodes(
    FabricCore::String const &newNodeNamesJSON,
    QPointF targetPos,
    unsigned &coreUndoCount
    );

  void moveNodes(
    FTL::ArrayRef<FTL::CStrRef> nodeNames,
    FTL::ArrayRef<QPointF> newTopLeftPoss,
    unsigned &coreUndoCount
    );

  QPointF getNodeUIGraphPos( 
    FTL::CStrRef nodeName
    );

private:

  QString m_execPath;
  FabricCore::DFGExec m_exec;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Exec__
