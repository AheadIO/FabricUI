// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGCommand::DFGCommand(DFGController * controller)
: GraphView::ControllerCommand(controller)
{
}

const char * DFGCommand::getExecPath()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getExecPath();
}

bool DFGCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return FabricCore::DFGHost(ctrl->getCoreDFGHost()).maybeUndo();  
}

bool DFGCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return FabricCore::DFGHost(ctrl->getCoreDFGHost()).maybeRedo();  
}
