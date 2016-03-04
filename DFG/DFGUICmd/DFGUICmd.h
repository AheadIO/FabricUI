// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd__
#define __UI_DFG_DFGUICmd__

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGUINamespace.h>
#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <QtGui/QUndoCommand>

FABRIC_UI_DFG_NAMESPACE_BEGIN

#define DFG_CMD_NAME(n) FTL_STR("FabricCanvas" n)

class DFGUICmd
{
public:

  DFGUICmd(
    FabricCore::DFGHost const &host
    )
    : m_host( host )
    , m_coreUndoCount( 0 )
    , m_state( State_New )
    {}
  virtual ~DFGUICmd() {}

  virtual void doit();

  virtual void undo();

  virtual void redo();

  QString getDesc()
  {
    assert( wasInvoked() );
    QString desc;
    desc += "Canvas: ";
    appendDesc( desc );
    return desc;
  }

protected:

  bool wasInvoked() const
    { return m_state != State_New; }

  virtual void appendDesc( QString &desc )
  {
    // [pzion 20151216] This should be abstract, but Shiboken can't deal
    // with abstract classes
  }

  virtual void invoke( unsigned &coreUndoCount )
  {
    // [pzion 20151216] This should be abstract, but Shiboken can't deal
    // with abstract classes
  }

  static void AppendDesc_String(
    QString string,
    QString &desc
    )
  {
    desc += '\'';
    desc += string;
    desc += '\'';
  }

  static void AppendDesc_String(
    char const *string,
    QString &desc
    )
  {
    desc += '\'';
    desc += string;
    desc += '\'';
  }

private:

  enum State
  {
    State_New,
    State_Done,
    State_Undone,
    State_Redone
  };

  FabricCore::DFGHost m_host;
  unsigned m_coreUndoCount;
  State m_state;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd__
