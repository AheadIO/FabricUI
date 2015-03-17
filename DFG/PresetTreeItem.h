// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_PresetTreeItem__
#define __UI_DFG_PresetTreeItem__

#include <DFGWrapper/Object.h>
#include <TreeView/TreeItem.h>

namespace FabricUI
{

  namespace DFG
  {

    class PresetTreeItem : public TreeView::TreeItem
    {

    public:

      PresetTreeItem(FabricServices::DFGWrapper::Object preset);

      virtual Qt::ItemFlags flags();
      virtual QString mimeDataAsText();

    private:

      QString m_path;
    };

  };

};

#endif // __UI_DFG_PresetTreeItem__
