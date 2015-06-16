// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGSetNodeCacheRuleCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetNodeCacheRuleCommand::DFGSetNodeCacheRuleCommand(DFGController * controller, char const * path, FEC_DFGCacheRule rule)
: DFGCommand(controller)
{
  m_nodePath = path;
  m_rule = rule;
}

bool DFGSetNodeCacheRuleCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec exec = ctrl->getCoreDFGExec();
  FabricCore::DFGExec subExec = exec.getSubExec(m_nodePath.c_str());

  if(m_rule == subExec.getCacheRule())
    exec.setInstCacheRule(m_nodePath.c_str(), FEC_DFGCacheRule_Unspecified);
  else
    exec.setInstCacheRule(m_nodePath.c_str(), m_rule);
  return true;
}

char const * DFGSetNodeCacheRuleCommand::getPath() const
{
  return m_nodePath.c_str();
}

FEC_DFGCacheRule DFGSetNodeCacheRuleCommand::getRule() const
{
  return m_rule;
}

char const * DFGSetNodeCacheRuleCommand::getRuleName() const
{
  if(m_rule == FEC_DFGCacheRule_Unspecified)
    return "Unspecified";
  if(m_rule == FEC_DFGCacheRule_Always)
    return "Always";
  if(m_rule == FEC_DFGCacheRule_Never)
    return "Never";
  return "";
}
