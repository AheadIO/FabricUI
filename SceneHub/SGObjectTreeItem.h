// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneHub_SGObjectTreeItem__
#define __UI_SceneHub_SGObjectTreeItem__

#include <FabricUI/TreeView/TreeItem.h>
#include <FabricCore.h>

namespace FabricUI
{

  namespace SceneHub
  {
    class SGObjectTreeView;
    
    class SGObjectTreeItem : public TreeView::TreeItem
    {
      Q_OBJECT

    public:

      static SGObjectTreeItem * Create(
        SGObjectTreeView * view,
        FTL::StrRef name,
        FabricCore::Client * client,
        FabricCore::RTVal sgObject
        );
      virtual ~SGObjectTreeItem();

      virtual FTL::CStrRef type() const { return FTL_STR("SGObject"); }

      virtual unsigned int numChildren();

    protected:

      SGObjectTreeItem(
        SGObjectTreeView * view,
        FTL::StrRef name,
        FabricCore::Client * client,
        FabricCore::RTVal browser
        );

      FabricCore::Client * m_client;
      FabricCore::RTVal m_rtVal;
      SGObjectTreeView * m_view;
    };

  };

};

#endif // __UI_SceneHub_SGObjectTreeItem__
