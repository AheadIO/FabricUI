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

  std::string getDesc()
  {
    assert( wasInvoked() );
    std::string desc;
    desc += FTL_STR("Canvas: ");
    appendDesc( desc );
    return desc;
  }

protected:

  bool wasInvoked() const
    { return m_state != State_New; }

  virtual void appendDesc( std::string &desc ) = 0;

  virtual void invoke( unsigned &coreUndoCount ) = 0;

  static void AppendDesc_String(
    FTL::CStrRef string,
    std::string &desc
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
