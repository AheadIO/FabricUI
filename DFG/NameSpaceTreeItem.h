// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_NameSpaceTreeItem__
#define __UI_DFG_NameSpaceTreeItem__

#include <FabricUI/TreeView/TreeItem.h>
#include <FabricCore.h>

namespace FabricUI
{

  namespace DFG
  {

    class NameSpaceTreeItem : public TreeView::TreeItem
    {

    public:

      NameSpaceTreeItem(FabricCore::DFGHost const &coreDFGHost, char const * name, char const * ns, QStringList filters = QStringList());

      virtual FTL::CStrRef type() const { return FTL_STR("NameSpace"); }

      virtual unsigned int numChildren();

      // filtering
      virtual QStringList filters() const;
      virtual void setFilters(QStringList f);

      // show presets
      virtual bool showsPresets() const { return m_showsPresets; }
      virtual void setShowsPresets(bool state) { m_showsPresets = state; }

    private:

      bool includeChildName(QString name);

      FabricCore::DFGHost m_coreDFGHost;
      std::string m_nameSpace;
      bool m_validated;
      bool m_showsPresets;
      QStringList m_filters;
    };

  };

};

#endif // __UI_DFG_NameSpaceTreeItem__
