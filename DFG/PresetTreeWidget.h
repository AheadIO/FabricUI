// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_PresetTreeWidget__
#define __UI_DFG_PresetTreeWidget__

#include <DFGWrapper/Host.h>

#include <QtGui/QWidget>
#include <TreeView/TreeViewWidget.h>
#include <TreeView/TreeModel.h>
#include "DFGConfig.h"

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

    private:

      FabricServices::DFGWrapper::Host * m_host;
      TreeView::TreeViewWidget * m_treeView;
      TreeView::TreeModel * m_treeModel;
    };

  };

};

#endif // __UI_DFG_PresetTreeWidget__
