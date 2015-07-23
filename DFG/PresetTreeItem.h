// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_PresetTreeItem__
#define __UI_DFG_PresetTreeItem__

#include <FabricUI/TreeView/TreeItem.h>

namespace FabricUI
{

  namespace DFG
  {

    class PresetTreeItem : public TreeView::TreeItem
    {

    public:

      PresetTreeItem(
        FTL::CStrRef path,
        FTL::CStrRef name
        );

      virtual FTL::CStrRef type() const { return FTL_STR("Preset"); }

      virtual Qt::ItemFlags flags();
      virtual QString mimeDataAsText();

    private:

      std::string m_path;
    };

  };

};

#endif // __UI_DFG_PresetTreeItem__
