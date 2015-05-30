// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRenameNodeCommand.h"
#include <DFGWrapper/Inst.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRenameNodeCommand::DFGRenameNodeCommand(DFGController * controller, QString path, QString title)
: DFGCommand(controller)
{
  m_nodePath = path;
  m_newTitle = title;
}

bool DFGRenameNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::NodePtr node = ctrl->getNodeFromPath(m_nodePath.toUtf8().constData());
  if ( !node->isInst() )
    return false;
  DFGWrapper::InstPtr inst = DFGWrapper::InstPtr::StaticCast( node );
  inst->setTitle(m_newTitle.toUtf8().constData());
  return true;
}

bool DFGRenameNodeCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();
}

bool DFGRenameNodeCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

QString DFGRenameNodeCommand::getPath() const
{
  return m_nodePath;
}

QString DFGRenameNodeCommand::getTitle() const
{
  return m_newTitle;
}
