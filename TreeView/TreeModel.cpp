// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <QtCore/QMimeData>

#include "TreeModel.h"
#include "WidgetTreeItem.h"

using namespace FabricUI::TreeView;

TreeModel::TreeModel(QObject * parent)
:QAbstractItemModel(parent)
{
}

TreeModel::~TreeModel()
{
  for(size_t i=0;i<m_items.size();i++)
    delete(m_items[i]);
}

unsigned int TreeModel::numItems()
{
  return m_items.size();
}

void TreeModel::addItem(TreeItem * itemToAdd)
{
  beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
  itemToAdd->setParent(NULL);
  itemToAdd->setModel(this);
  itemToAdd->setIndex(m_items.size());
  m_items.push_back(itemToAdd);
  endInsertRows();
}

bool TreeModel::removeItem(TreeItem * itemToRemove)
{
  unsigned int index = m_items.size();
  for(unsigned int i=0;i<m_items.size();i++)
  {
    if(m_items[i] == itemToRemove)
    {
      index = i;
      break;
    }
  }

  if(index == m_items.size())
    return false;

  beginRemoveRows(QModelIndex(), index, index);
  m_items.erase(m_items.begin() + index);
  endRemoveRows();
  return true;
}

TreeItem * TreeModel::item(unsigned int index)
{
  return m_items[index];
}

TreeItem * TreeModel::item( FTL::StrRef path )
{
  FTL::StrRef::Split split = path.split('.');
  if ( !split.first.empty() )
  {
    for ( size_t i = 0; i < m_items.size(); ++i )
    {
      if ( m_items[i]->name() == split.first )
      {
        if ( split.second.empty() )
          return m_items[i];
        else
          return m_items[i]->child( split.second );
      }
    }
  }
  return 0;
}

TreeItem * TreeModel::item( QModelIndex index )
{
  for ( size_t i = 0; i < m_items.size(); ++i )
  {
    TreeItem * candidate = m_items[i];
    if ( candidate->modelIndex() == index )
      return candidate;
    candidate = candidate->child( index );
    if(candidate)
      return candidate;
  }
  return NULL;  
}

void TreeModel::clear()
{
  size_t prevSize = m_items.size();
  if(prevSize == 0)
    return;

  beginRemoveRows(QModelIndex(), 0, prevSize - 1);
  for(size_t i=0;i<prevSize;i++)
    delete(m_items[i]);
  m_items.clear();
  endRemoveRows();
}

int TreeModel::columnCount(const QModelIndex & parent) const
{
  return 1;
}

QVariant TreeModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid())
    return QVariant();

  TreeItem * item = (TreeItem*)index.internalPointer();
  return item->data(role);
}

void TreeModel::fetchMore(const QModelIndex & parent)
{
}

bool TreeModel::hasChildren(const QModelIndex & parent) const
{
  return rowCount(parent) > 0;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex & parent) const
{
  if(!parent.isValid())
  {
    if(size_t(row) >= m_items.size())
      return QModelIndex();

    QModelIndex idx = m_items[row]->modelIndex();
    if(!idx.isValid())
    {
      idx = createIndex(row, column, m_items[row]);
      m_items[row]->setModelIndex(idx);
    }
    return idx;
  }

  TreeItem * parentItem = (TreeItem *)parent.internalPointer();
  if(size_t(row) >= parentItem->numChildren())
    return QModelIndex();

  TreeItem * item = parentItem->child(row);
  QModelIndex idx = item->modelIndex();
  if(!idx.isValid())
  {
    idx = createIndex(row, column, item);
    item->setModelIndex(idx);
  }
  return idx;
}

QModelIndex TreeModel::parent(const QModelIndex & index) const
{
  if(!index.isValid())
    return QModelIndex();

  TreeItem * item = (TreeItem *)index.internalPointer();
  TreeItem * parent = item->parent();
  if(!parent)
    return QModelIndex();
  QModelIndex idx = parent->modelIndex();
  if(!idx.isValid())
  {
    idx = createIndex(parent->index(), 0, parent);
    parent->setModelIndex(idx);
  }
  return idx;
}

int TreeModel::rowCount(const QModelIndex & parent) const
{
  if(!parent.isValid())
    return ((TreeModel*)this)->numItems();

  TreeItem * parentItem = (TreeItem *)parent.internalPointer();
  return parentItem->numChildren();
}

void TreeModel::invalidateItem(TreeItem * item)
{
  emit dataChanged(item->modelIndex(), item->modelIndex());
}

void TreeModel::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
  for(size_t i=0;i<m_items.size();i++)
    m_items[i]->selectionChanged(selected, deselected);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags result = QAbstractItemModel::flags(index);
  TreeItem * item = (TreeItem *)index.internalPointer();
  if(item)
    result = result | item->flags();
  return result;
}

QMimeData * TreeModel::mimeData(const QModelIndexList &indexes) const
{
  QMimeData *mimeData = new QMimeData();

  QString text;
  text += "[";

  bool hasComma = false;
  foreach(QModelIndex index, indexes)
  {
    if (index.isValid())
    {
      TreeItem * item = (TreeItem *)index.internalPointer();
      if(hasComma)
        text += ", ";
      if(item)
        text += item->mimeDataAsText();
      hasComma = true;
    }
  }

  text += "]";

  mimeData->setText(text);
  return mimeData;
}
