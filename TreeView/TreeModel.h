// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_TreeModel__
#define __UI_TreeView_TreeModel__

#include <QtGui/QTreeView>

#include "TreeItem.h"
#include <vector>

namespace FabricUI
{

  namespace TreeView
  {
    class TreeModel : public QAbstractItemModel
    {
      friend class TreeViewWidget;
      friend class TreeItem;

      Q_OBJECT

    public:
      TreeModel(QObject * parent);
      virtual ~TreeModel();

      // item management
      virtual unsigned int numItems();
      virtual void addItem(TreeItem * itemToAdd);
      virtual bool removeItem(TreeItem * itemToRemove);
      virtual TreeItem * item(unsigned int index);
      virtual TreeItem * item( FTL::StrRef path );
      virtual TreeItem * item( QModelIndex index );
      virtual void clear();

      // qt model implementation
      virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
      virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
      virtual void fetchMore(const QModelIndex & parent);
      virtual bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
      virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
      virtual QModelIndex parent(const QModelIndex & index) const;
      virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
      virtual Qt::ItemFlags flags(const QModelIndex &index) const;
      virtual QMimeData * mimeData(const QModelIndexList &indexes) const;

    public slots:

      void invalidateItem(TreeItem * item);

    signals:

      void itemSelected(FabricUI::TreeView::TreeItem * item);
      void itemDeselected(FabricUI::TreeView::TreeItem * item);
      void itemExpanded(FabricUI::TreeView::TreeItem * item);
      void itemCollapsed(FabricUI::TreeView::TreeItem * item);

    protected:

      void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
      std::vector<TreeItem*> m_items;
    };

  };

};

#endif // __UI_TreeView_TreeModel__
