// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_PresetTreeWidget__
#define __UI_DFG_PresetTreeWidget__

#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <FabricUI/TreeView/TreeViewWidget.h>
#include <FabricUI/TreeView/TreeModel.h>
#include <FabricUI/TreeView/TreeItem.h>
#include "DFGConfig.h"
#include <SplitSearch/SplitSearch.hpp>

namespace FabricUI
{

  namespace DFG
  {
    class DFGController;

    class PresetTreeWidget : public QWidget
    {
      Q_OBJECT

    public:

      PresetTreeWidget(
        DFGController *dfgController,
        const DFGConfig & config, 
        bool showsPresets = true,
        bool showSearch = true,
        bool setupContextMenu = false
        );
      virtual ~PresetTreeWidget();

      TreeView::TreeViewWidget * getTreeView() { return m_treeView; }
      TreeView::TreeModel * getTreeModel() { return m_treeModel; }

    public slots:

      void refresh();
      void setBinding( FabricCore::DFGBinding const &binding );
      void onCustomContextMenuRequested(QPoint globalPos, FabricUI::TreeView::TreeItem * item);
      void onContextMenuAction(QAction * action);

    private:
      
      void updatePresetPathDB();

      DFGController *m_dfgController;
      QLineEdit * m_searchEdit;
      TreeView::TreeViewWidget * m_treeView;
      TreeView::TreeModel * m_treeModel;
      FabricServices::SplitSearch::Dict m_presetPathDict;
      std::vector<std::string> m_presetPathDictSTL;
      bool m_presetDictsUpToDate;
      QString m_state;
      bool m_showsPresets;
      std::string m_contextPath;
    };

  };

};

#endif // __UI_DFG_PresetTreeWidget__
