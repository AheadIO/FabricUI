// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGSetNodeCacheRuleCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetNodeCacheRuleCommand::DFGSetNodeCacheRuleCommand(DFGController * controller, QString path, FEC_DFGCacheRule rule)
: DFGCommand(controller)
{
  m_nodePath = path;
  m_rule = rule;
}

bool DFGSetNodeCacheRuleCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::NodePtr node = ctrl->getNodeFromPath(m_nodePath.toUtf8().constData());
  if(m_rule == node->getExecutable()->getCacheRule())
    node->setCacheRule(FEC_DFGCacheRule_Unspecified);
  else
    node->setCacheRule(m_rule);
  return true;
}

bool DFGSetNodeCacheRuleCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();
}

bool DFGSetNodeCacheRuleCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

QString DFGSetNodeCacheRuleCommand::getPath() const
{
  return m_nodePath;
}

FEC_DFGCacheRule DFGSetNodeCacheRuleCommand::getRule() const
{
  return m_rule;
}

QString DFGSetNodeCacheRuleCommand::getRuleName() const
{
  if(m_rule == FEC_DFGCacheRule_Unspecified)
    return "Unspecified";
  if(m_rule == FEC_DFGCacheRule_Always)
    return "Always";
  if(m_rule == FEC_DFGCacheRule_Never)
    return "Never";
  return "";
}
