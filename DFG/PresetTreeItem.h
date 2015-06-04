// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_PresetTreeItem__
#define __UI_DFG_PresetTreeItem__

#include <TreeView/TreeItem.h>

namespace FabricUI
{

  namespace DFG
  {

    class PresetTreeItem : public TreeView::TreeItem
    {

    public:

      PresetTreeItem(char const * path, char const * name);

      virtual Qt::ItemFlags flags();
      virtual QString mimeDataAsText();

    private:

      std::string m_path;
    };

  };

};

#endif // __UI_DFG_PresetTreeItem__
