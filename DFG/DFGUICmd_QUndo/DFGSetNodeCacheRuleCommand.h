// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGSetNodeCacheRuleCommand__
#define __UI_DFG_DFGSetNodeCacheRuleCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGSetNodeCacheRuleCommand: public DFGCommand
    {
    public:

      DFGSetNodeCacheRuleCommand(DFGController * controller, char const * path, FEC_DFGCacheRule rule);

      virtual const char * getName() const { return "dfgSetNodeCacheRule"; }
      virtual const char * getShortDesc() const { return "Changes the cache rule for a given node."; }
      virtual const char * getFullDesc() const { return "Changes the cache rule for a given node."; }

      char const * getPath() const;
      FEC_DFGCacheRule getRule() const;
      char const * getRuleName() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_nodePath;
      FEC_DFGCacheRule m_rule;

    };

  };

};

#endif // __UI_DFG_DFGSetNodeCacheRuleCommand__
