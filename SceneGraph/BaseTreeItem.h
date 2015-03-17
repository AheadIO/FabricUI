// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneGraph_BaseTreeItem__
#define __UI_SceneGraph_BaseTreeItem__

#include <TreeView/TreeItem.h>
#include <FabricCore.h>

namespace FabricUI
{

  namespace SceneGraph
  {
    class BaseTreeItem : public TreeView::TreeItem
    {
      Q_OBJECT

  public:

      BaseTreeItem(QString name, FabricCore::RTVal rtVal);
      virtual ~BaseTreeItem();

      virtual QString type() const;

      virtual unsigned int numChildren();

      FabricCore::RTVal m_rtVal;
    };

  };

};

#endif // __UI_SceneGraph_BaseTreeItem__
