// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_VariableListTreeItem__
#define __UI_DFG_VariableListTreeItem__

#include <FabricUI/TreeView/TreeItem.h>
#include <FabricCore.h>

namespace FabricUI
{

  namespace DFG
  {

    class VariableListTreeItem : public TreeView::TreeItem
    {

    public:

      VariableListTreeItem(FabricCore::DFGHost const &coreDFGHost, FabricCore::DFGBinding const &coreDFGBinding, QStringList filters = QStringList());

      virtual unsigned int numChildren();

      // filtering
      virtual QStringList filters() const;
      virtual void setFilters(QStringList f);

    private:

      bool includeChildName(QString name);

      FabricCore::DFGHost m_coreDFGHost;
      FabricCore::DFGBinding m_coreDFGBinding;
      QStringList m_filters;
    };

  };

};

#endif // __UI_DFG_VariableListTreeItem__
