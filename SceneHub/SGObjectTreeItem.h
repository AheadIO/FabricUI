// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneHub_SGObjectTreeItem__
#define __UI_SceneHub_SGObjectTreeItem__

#include <TreeView/TreeItem.h>
#include <FabricCore.h>

namespace FabricUI
{

  namespace SceneHub
  {
    class SGObjectTreeItem : public TreeView::TreeItem
    {
      Q_OBJECT

    public:

      static SGObjectTreeItem * Create(QString name, FabricCore::Client * client, FabricCore::RTVal sgObject);
      virtual ~SGObjectTreeItem();

      virtual QString type() const;
      virtual unsigned int numChildren();

    protected:

      SGObjectTreeItem(QString name, FabricCore::Client * client, FabricCore::RTVal browser);

      FabricCore::Client * m_client;
      FabricCore::RTVal m_rtVal;
    };

  };

};

#endif // __UI_SceneHub_SGObjectTreeItem__
