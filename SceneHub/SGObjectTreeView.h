// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneHub_SGObjectTreeView__
#define __UI_SceneHub_SGObjectTreeView__

#include <QtCore/QPoint>
#include <QtGui/QAction>

#include <TreeView/TreeViewWidget.h>
#include <TreeView/TreeItem.h>

namespace FabricUI
{

  namespace SceneHub
  {
    class SGObjectTreeView : public TreeView::TreeViewWidget
    {
      Q_OBJECT

      friend class SGObjectTreeItem;

    public:

      SGObjectTreeView(QWidget * parent);
      virtual ~SGObjectTreeView();

      virtual void showCustomContextMenu(const QPoint & globalPos, TreeView::TreeItem * item);

    public slots:

      void contextMenuTriggered(QAction * action);

    signals:

      void itemExpanded(FabricUI::TreeView::TreeItem *);

    private:

      TreeView::TreeItem * m_lastItemClickedOn;
    };

  };

};

#endif // __UI_SceneHub_SGObjectTreeView__
