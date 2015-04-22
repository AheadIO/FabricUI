// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_TreeItem__
#define __UI_TreeView_TreeItem__

#include <QtGui/QWidget>
#include <QtGui/QItemSelection>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItem>
#include <QtCore/QEvent>

#include <vector>

namespace FabricUI
{

  namespace TreeView
  {
    enum TreeItemType
    {
      TreeItemType_Default,
      TreeItemType_Widget
    };

    class TreeModel;

    class TreeItem : public QObject
    {
      friend class TreeViewWidget;
      friend class TreeModel;

      Q_OBJECT

    public:

      TreeItem(QString name, QString type = "item", QString label = "");
      virtual ~TreeItem();

      virtual TreeItemType itemType() const;
      virtual bool editable() const;
      virtual void setEditable(bool state);
      virtual QString name() const;
      virtual QString label() const;
      virtual QString path() const;
      virtual QString type() const;
      virtual QColor foregroundColor() const;
      virtual void setForegroundColor(QColor color);
      virtual QColor backgroundColor() const;
      virtual void setBackgroundColor(QColor color);
      virtual QPixmap pixmap() const;
      virtual void setPixmap(QPixmap pm);
      virtual unsigned int index() const;
      virtual QModelIndex modelIndex(TreeModel * model = NULL) const;
      virtual bool selected() const;
      virtual bool collapsed() const;
      virtual bool expanded() const;

      virtual unsigned int numChildren();
      virtual void addChild(TreeItem * childToAdd);
      virtual bool removeChild(TreeItem * childToRemove);
      virtual TreeItem * child(unsigned int i);
      virtual TreeItem * child(QString path);
      virtual TreeItem * parent();

      virtual Qt::ItemFlags flags();
      virtual QVariant data(int role);
      virtual QString mimeDataAsText();

    public slots:

      void onChildAdded(FabricUI::TreeView::TreeItem * child);
      void onChildRemoved(FabricUI::TreeView::TreeItem * child);

    signals:

      void childAdded(FabricUI::TreeView::TreeItem*);
      void childRemoved(FabricUI::TreeView::TreeItem*);
      void childAddedRecursive(FabricUI::TreeView::TreeItem*);
      void childRemovedRecursive(FabricUI::TreeView::TreeItem*);

    protected:

      virtual void setIndex(unsigned int i);
      virtual void setParent(TreeItem * item);
      virtual void setSelected(bool state);
      virtual void setExpanded(bool state);
      virtual void setModelIndex(QModelIndex modelIndex);
      void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    private:

      QString m_name;
      QString m_label;
      QString m_type;
      QColor m_backgroundColor;
      QColor m_foregroundColor;
      QPixmap m_pixmap;
      TreeItem * m_parent;
      unsigned int m_index;
      mutable QModelIndex m_modelIndex;
      bool m_selected;
      bool m_expanded;
      bool m_editable;
      std::vector<TreeItem*> m_children;
    };

  };

};

#include "TreeModel.h"

#endif // __UI_TreeView_TreeItem__
