// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_VariableGroupTreeItem__
#define __UI_DFG_VariableGroupTreeItem__

#include <FabricUI/TreeView/TreeItem.h>
#include <FabricCore.h>
#include <FTL/JSONValue.h>

namespace FabricUI
{

  namespace DFG
  {

    class VariableGroupTreeItem : public TreeView::TreeItem
    {

    public:

      VariableGroupTreeItem(char const * name, char const * path, FTL::JSONObject const * jsonObject, QStringList filters = QStringList());

      virtual unsigned int numChildren();

      // filtering
      virtual QStringList filters() const;
      virtual void setFilters(QStringList f);

    private:

      bool includeChildName(QString name);

      FabricCore::DFGBinding m_coreDFGBinding;
      QStringList m_filters;
      std::string m_name;
      std::string m_path;

    protected:
      FTL::JSONObject const * m_jsonObject;
    };

  };

};

#endif // __UI_DFG_VariableGroupTreeItem__
