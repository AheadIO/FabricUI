// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_QUndo__
#define __UI_DFG_DFGUICmd_QUndo__

#include <FabricUI/DFG/DFGUINamespace.h>
#include <FabricCore.h>
#include <QtGui/QUndoCommand>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_QUndo : public QUndoCommand
{
public:

  DFGUICmd_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGHost const &host
    )
    : QUndoCommand()
    , m_host( host )
    , m_coreUndoCount( 0 )
    { setText( desc.c_str() ); }

protected:

  virtual void redo()
  {
    if ( m_coreUndoCount )
    {
      for ( unsigned i = 0; i < m_coreUndoCount; ++i )
        m_host.maybeRedo();
    }
    else
    {
      try
      {
        invokeForHost(
          m_host,
          m_coreUndoCount
          );
      }
      catch ( FabricCore::Exception e )
      {
        printf(
          "Caught FabricCore::Exception: %s\n",
          e.getDesc_cstr()
          );
      }
    }
  }

  virtual void undo()
  {
    for ( unsigned i = 0; i < m_coreUndoCount; ++i )
      m_host.maybeUndo();
  }

  virtual void invokeForHost(
    FabricCore::DFGHost &host,
    unsigned &coreUndoCount
    ) = 0;

private:

  FabricCore::DFGHost m_host;
  unsigned m_coreUndoCount;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_QUndo__
