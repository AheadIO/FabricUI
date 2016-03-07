//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Binding.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_DismissLoadDiags
  : public DFGUICmd_Binding
{
public:

  DFGUICmd_DismissLoadDiags(
    FabricCore::DFGBinding const &binding,
    QList<int> diagIndices
    )
    : DFGUICmd_Binding(
      binding
      )
    , m_diagIndices( diagIndices )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("DismissLoadDiags"); }

protected:

  virtual void appendDesc( QString &desc );

  virtual void invoke( unsigned &coreUndoCount );

  static void Perform(
    FabricCore::DFGBinding &binding,
    QList<int> diagIndices,
    unsigned &coreUndoCount
    );

private:

  QList<int> m_diagIndices;
};

FABRIC_UI_DFG_NAMESPACE_END
