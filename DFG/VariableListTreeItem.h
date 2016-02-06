// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_VariableListTreeItem__
#define __UI_DFG_VariableListTreeItem__

#include "VariableGroupTreeItem.h"
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>

namespace FabricUI
{

  namespace DFG
  {

    class VariableListTreeItem : public VariableGroupTreeItem
    {

    public:

      VariableListTreeItem(FabricCore::DFGBinding const &coreDFGBinding, QStringList filters = QStringList());

      virtual FTL::CStrRef type() const { return FTL_STR("VariableList"); }

      FTL::OwnedPtr<FTL::JSONObject> m_json;
      FabricCore::DFGBinding m_coreDFGBinding;
    };

  };

};

#endif // __UI_DFG_VariableListTreeItem__
