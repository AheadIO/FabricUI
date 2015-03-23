// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_PresetTreeWidget__
#define __UI_DFG_PresetTreeWidget__

#include <DFGWrapper/Host.h>

#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <TreeView/TreeViewWidget.h>
#include <TreeView/TreeModel.h>
#include "DFGConfig.h"
#include <SplitSearch/SplitSearch.hpp>

namespace FabricUI
{

  namespace DFG
  {

    class PresetTreeWidget : public QWidget
    {
      Q_OBJECT

    public:

      PresetTreeWidget(QWidget * parent, FabricServices::DFGWrapper::Host * host, const DFGConfig & config = DFGConfig());
      virtual ~PresetTreeWidget();

    public slots:

      void refresh();
      void searchChanged(const QString & text);

    private:
      
      void updatePresetPathDB();

      QLineEdit * m_searchEdit;
      FabricServices::DFGWrapper::Host * m_host;
      TreeView::TreeViewWidget * m_treeView;
      TreeView::TreeModel * m_treeModel;
      FabricServices::SplitSearch::Dict m_presetNameSpaceDict;
      FabricServices::SplitSearch::Dict m_presetPathDict;
      std::vector<std::string> m_presetNameSpaceDictSTL;
      std::vector<std::string> m_presetPathDictSTL;
      bool m_presetDictsUpToDate;
    };

  };

};

#endif // __UI_DFG_PresetTreeWidget__
