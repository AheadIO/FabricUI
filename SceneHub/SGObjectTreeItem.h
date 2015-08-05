// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

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

      friend class SGObjectTreeModel;

    public:

      static SGObjectTreeItem * Create(SGObjectTreeView * view, QString name, FabricCore::Client * client, FabricCore::RTVal sgObject);
      virtual ~SGObjectTreeItem();

      virtual QString type() const;
      virtual unsigned int numChildren();
      virtual void loadRecursively();

      virtual TreeItem * child(unsigned int i);
      virtual TreeItem * child(QString path);

      void updateChildrenCount();

    protected:

      SGObjectTreeItem(SGObjectTreeView * view, QString name, FabricCore::Client * client, FabricCore::RTVal browser);
      void createChildrenIfNotDone();
      void updateRecursively();

      FabricCore::Client * m_client;
      FabricCore::RTVal m_rtVal;
      SGObjectTreeView * m_view;
      bool m_childrenCreated;
    };

  };

};

#endif // __UI_SceneHub_SGObjectTreeItem__
