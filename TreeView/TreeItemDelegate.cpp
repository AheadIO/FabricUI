// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "TreeItemDelegate.h"
#include "TreeViewWidget.h"
#include "TreeItem.h"
#include "WidgetTreeItem.h"

using namespace FabricUI::TreeView;

TreeItemDelegate::TreeItemDelegate(TreeViewWidget * parent)
: QStyledItemDelegate(parent)
{
  m_view = parent;
  m_factory = NULL;
}

TreeItemDelegate::~TreeItemDelegate()
{
}

QWidget * TreeItemDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  if(index.isValid() && m_factory)
  {
    TreeItem * item = (TreeItem *)index.internalPointer();
    if(item)
    {
      if(item->itemType() == TreeItemType_Widget)
      {
        WidgetTreeItem * widgetItem = (WidgetTreeItem *)item;
        TreeEditorWidget * widget = m_factory->createEditor(parent, widgetItem);
        if(widget)
          return widget;
      }
    }
    
  }
  return QStyledItemDelegate::createEditor(parent, option, index);
}

void TreeItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  if(!index.isValid())
    return;
  TreeItem * item = (TreeItem *)index.internalPointer();
  if(!item)
    return;
  if(item->itemType() != TreeItemType_Widget)
    return;
  ((WidgetTreeItem*)item)->paint(painter, option);
}

QSize TreeItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  if(!index.isValid())
    return QSize();
  TreeItem * item = (TreeItem *)index.internalPointer();
  if(!item)
    return QSize();
  if(item->itemType() != TreeItemType_Widget)
    return QSize();
  return ((WidgetTreeItem*)item)->sizeHint(option);
}

void TreeItemDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  editor->setGeometry(option.rect);
}

TreeEditorFactory * TreeItemDelegate::factory()
{
  return m_factory;
}

void TreeItemDelegate::setFactory(TreeEditorFactory * factory)
{
  m_factory = factory;
}
