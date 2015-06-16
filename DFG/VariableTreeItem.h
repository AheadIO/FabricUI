// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_VariableTreeItem__
#define __UI_DFG_VariableTreeItem__

#include <FabricUI/TreeView/TreeItem.h>

namespace FabricUI
{

  namespace DFG
  {

    class VariableTreeItem : public TreeView::TreeItem
    {

    public:

      VariableTreeItem(char const * name, char const * path);

      virtual Qt::ItemFlags flags();
      virtual QString mimeDataAsText();

    private:

      std::string m_name;
      std::string m_path;
    };

  };

};

#endif // __UI_DFG_VariableTreeItem__
