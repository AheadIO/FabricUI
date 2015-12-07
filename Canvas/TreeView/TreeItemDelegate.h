// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_TreeItemDelegate__
#define __UI_TreeView_TreeItemDelegate__

#include <QtGui/QWidget>
#include <QtGui/QStyledItemDelegate>

#include <vector>

#include "TreeEditorFactory.h"

namespace FabricUI
{

  namespace TreeView
  {
    // forward declarations
    class TreeViewWidget;
    class TreeItem;

    class TreeItemDelegate : public QStyledItemDelegate
    {
      friend class TreeViewWidget;

    public:

      TreeItemDelegate(TreeViewWidget * parent);
      virtual ~TreeItemDelegate();

      TreeEditorFactory * factory();

      virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
      virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
      virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
      virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

    protected:

      void setFactory(TreeEditorFactory * factory);
      
    private:

      TreeViewWidget * m_view;
      TreeEditorFactory * m_factory;
    };

  };

};

#endif // __UI_TreeView_TreeItemDelegate__
