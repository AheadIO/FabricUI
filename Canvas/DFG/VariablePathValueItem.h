// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_VariablePathValueItem__
#define __UI_DFG_VariablePathValueItem__

#include <FabricUI/Canvas/ValueEditor/ValueItem.h>
#include <map>

#include <FabricCore.h>
#include <FTL/CStrRef.h>

namespace FabricUI
{

  namespace DFG
  {
    class VariablePathValueItem : public ValueEditor::ValueItem
    {
      Q_OBJECT

    public:

      VariablePathValueItem(
        FTL::StrRef name,
        TreeView::TreeEditorFactory * factory,
        FabricCore::Client * client,
        FabricCore::DFGBinding binding,
        FTL::CStrRef execPath,
        QWidget * parent,
        FabricCore::RTVal value,
        FTL::StrRef label = FTL::StrRef(),
        bool enabled = true
        );

      virtual FTL::CStrRef type() const { return FTL_STR("VariablePath"); }

      FabricCore::DFGBinding getDFGBinding() { return m_binding; }
      FTL::CStrRef getDFGExecPath() { return m_execPath.c_str(); }

    private:

      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
    };

  };

};

#endif // __UI_DFG_VariablePathValueItem__
