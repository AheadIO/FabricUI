// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetArgType_QUndo__
#define __UI_DFG_DFGUICmd_SetArgType_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Binding_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetArgType_QUndo
  : public DFGUICmd_Binding_QUndo
{
public:

  DFGUICmd_SetArgType_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef argName,
    FTL::StrRef typeName
    )
    : DFGUICmd_Binding_QUndo(
      desc,
      binding
      )
    , m_argName( argName )
    , m_typeName( typeName )
    {}

protected:
  
  virtual void invokeForBinding(
    FabricCore::DFGBinding &binding,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_SetArgType(
      binding,
      m_argName,
      m_typeName,
      coreUndoCount
      );
  }

private:

  std::string m_argName;
  std::string m_typeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetArgType_QUndo__
