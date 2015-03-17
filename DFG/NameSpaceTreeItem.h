// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_NameSpaceTreeItem__
#define __UI_DFG_NameSpaceTreeItem__

#include <DFGWrapper/NameSpace.h>
#include <TreeView/TreeItem.h>

namespace FabricUI
{

  namespace DFG
  {

    class NameSpaceTreeItem : public TreeView::TreeItem
    {

    public:

      NameSpaceTreeItem(FabricServices::DFGWrapper::NameSpace nameSpace);

      virtual unsigned int numChildren();

    private:

      FabricServices::DFGWrapper::NameSpace m_nameSpace;
      bool m_validated;
    };

  };

};

#endif // __UI_DFG_NameSpaceTreeItem__
