// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetArgValue_QUndo__
#define __UI_DFG_DFGUICmd_SetArgValue_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Binding_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetArgValue_QUndo
  : public DFGUICmd_Binding_QUndo
{
public:

  DFGUICmd_SetArgValue_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef argName,
    FabricCore::RTVal const &value
    )
    : DFGUICmd_Binding_QUndo(
      desc,
      binding
      )
    , m_argName( argName )
    , m_value( value )
    {}

protected:
  
  virtual void invokeForBinding(
    FabricCore::DFGBinding &binding,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_SetArgValue(
      binding,
      m_argName,
      m_value,
      coreUndoCount
      );
  }

private:

  std::string m_argName;
  FabricCore::RTVal m_value;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetArgValue_QUndo__
