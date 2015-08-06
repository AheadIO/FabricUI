// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_TreeItem__
#define __UI_TreeView_TreeItem__

#include <QtGui/QWidget>
#include <QtGui/QItemSelection>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItem>
#include <QtCore/QEvent>

#include <vector>

#include <FTL/CStrRef.h>

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

      TreeItem(
        FTL::StrRef name,
        FTL::StrRef label = FTL::StrRef()
        );
      virtual ~TreeItem();

      virtual FTL::CStrRef type() const { return FTL_STR("item"); }

      FTL::CStrRef name() const
        { return m_name; }
      FTL::CStrRef label() const
        { return m_label; }
      std::string path() const;

      virtual TreeItemType itemType() const;
      virtual bool editable() const;
      virtual void setEditable(bool state);
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
      virtual void removeChildrenRecursively();

      virtual TreeItem * child( unsigned int i );
      virtual TreeItem * child( FTL::StrRef path );
      virtual TreeItem * parent();
      virtual TreeModel * model();

      virtual Qt::ItemFlags flags();
      virtual QVariant data(int role);
      virtual QString mimeDataAsText();

    signals:

      void childAdded(FabricUI::TreeView::TreeItem*);
      void childRemoved(FabricUI::TreeView::TreeItem*);

    protected:

      virtual void setIndex(unsigned int i);
      virtual void setParent(TreeItem * item);
      virtual void setModel(TreeModel * model);
      virtual void setSelected(bool state);
      virtual void setExpanded(bool state);
      virtual void setModelIndex(QModelIndex modelIndex);
      virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    private:

      std::string m_name;
      std::string m_label;
      QColor m_backgroundColor;
      QColor m_foregroundColor;
      QPixmap m_pixmap;
      TreeItem * m_parent;
      TreeModel * m_model;
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
