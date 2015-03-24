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

      NameSpaceTreeItem(FabricServices::DFGWrapper::NameSpace nameSpace, QStringList filters = QStringList());

      virtual unsigned int numChildren();

      // filtering
      virtual QStringList filters() const;
      virtual void setFilters(QStringList f);

    private:

      bool includeChildName(QString name);

      FabricServices::DFGWrapper::NameSpace m_nameSpace;
      bool m_validated;
      QStringList m_filters;
    };

  };

};

#endif // __UI_DFG_NameSpaceTreeItem__
