// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_TreeViewWidget__
#define __UI_TreeView_TreeViewWidget__

#include <QtGui/QWidget>
#include <QtGui/QTreeView>

#include "TreeItem.h"
#include "TreeItemDelegate.h"
#include "TreeEditorFactory.h"

namespace FabricUI
{

  namespace TreeView
  {
    class TreeViewWidget : public QTreeView
    {
      Q_OBJECT

    public:
      TreeViewWidget(QWidget * parent, bool supportsDelegates = false);
      virtual ~TreeViewWidget();

      virtual void showCustomContextMenu(const QPoint & globalPos, TreeItem * item) {};

      virtual void setFactory(TreeEditorFactory * factory);

    private slots:
      void onCustomContextMenuRequested(const QPoint & point);
      void onEnteredIndex(const QModelIndex & index);
      void onExpanded(const QModelIndex & index);
      void onCollapsed(const QModelIndex & index);

    signals:

      void itemSelectionChanged();

    protected:

      void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    private:

      TreeItemDelegate * m_delegate;
      bool m_usesDelegate;
    };

  };

};

#endif // __UI_TreeView_TreeViewWidget__
