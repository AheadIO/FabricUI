// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
/*
#ifndef __UI_SceneHub_SGObjectTreeModel__
#define __UI_SceneHub_SGObjectTreeModel__


#include <FabricUI/TreeView/TreeModel.h>

namespace FabricUI
{

  namespace SceneHub
  {
    class SGObjectTreeModel : public TreeView::TreeModel
    {
      Q_OBJECT

      friend class SGObjectTreeItem;

    public:
      SGObjectTreeModel(QObject * parent);
      virtual ~SGObjectTreeModel();

    public slots:

      void onSceneHierarchyChanged();

    signals:

      void itemChanged(FabricUI::TreeView::TreeItem * item);

    private:
    };

  };

};


#endif // __UI_SceneHub_SGObjectTreeModel__
*/