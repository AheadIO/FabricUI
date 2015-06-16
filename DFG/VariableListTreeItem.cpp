// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "VariableListTreeItem.h"
#include "VariableTreeItem.h"
#include <map>

using namespace FabricUI;
using namespace FabricUI::DFG;

VariableListTreeItem::VariableListTreeItem(FabricCore::DFGBinding const &coreDFGBinding, QStringList filters)
: VariableGroupTreeItem("Variables", "", NULL, filters)
, m_coreDFGBinding(coreDFGBinding)
{
  if(m_coreDFGBinding.isValid())
  {
    FabricCore::DFGStringResult jsonStr =  m_coreDFGBinding.getVars();
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr.getCString() );
    m_json = FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>();
    m_jsonObject = m_json.get();
  }
}
