// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtCore/QMimeData>

#include "TreeModel.h"
#include "WidgetTreeItem.h"
#include <iostream>
using namespace FabricUI::TreeView;

TreeModel::TreeModel(QObject * parent)
:QAbstractItemModel(parent)
{
  std::cerr << "TreeModel::TreeModel" << std::endl;
}

TreeModel::~TreeModel()
{
  std::cerr << "TreeModel::~TreeModel" << std::endl;


  for(size_t i=0;i<m_items.size();i++)
    delete(m_items[i]);
}

unsigned int TreeModel::numItems()
{
  std::cerr << "TreeModel::numItems" << std::endl;


  return m_items.size();
}

void TreeModel::addItem(TreeItem * itemToAdd)
{
  std::cerr << "TreeModel::addItem" << std::endl;


  beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
  itemToAdd->setParent(NULL);
  itemToAdd->setModel(this);
  itemToAdd->setIndex(m_items.size());
  m_items.push_back(itemToAdd);
  endInsertRows();
}

bool TreeModel::removeItem(TreeItem * itemToRemove)
{
  std::cerr << "TreeModel::removeItem" << std::endl;


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
  std::cerr << "TreeModel::item 1" << std::endl;


  return m_items[index];
}

TreeItem * TreeModel::item(QString path)
{
  std::cerr << "TreeModel::item 2" << std::endl;


  int pos = path.indexOf('.');
  QString left = path;
  QString right;
  if(pos > -1)
  {
    left = path.left(pos);
    right = path.right(path.length()-pos-1);
  }
  for(size_t i=0;i<m_items.size();i++)
  {
    if(m_items[i]->path() == path)
      return m_items[i];
    if(m_items[i]->name() == left)
    {
      if(right.length() == 0)
        return m_items[i];
      return m_items[i]->child(right);
    }
  }
  return NULL;
}

void TreeModel::clear()
{
  std::cerr << "TreeModel::clear" << std::endl;


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
  std::cerr << "TreeModel::columnCount" << std::endl;

  return 1;
}

QVariant TreeModel::data(const QModelIndex & index, int role) const
{
  std::cerr << "TreeModel::data" << std::endl;

  if(!index.isValid())
    return QVariant();

  TreeItem * item = (TreeItem*)index.internalPointer();
  return item->data(role);
}

void TreeModel::fetchMore(const QModelIndex & parent)
{
  std::cerr << "TreeModel::fetchMore" << std::endl;
}

bool TreeModel::hasChildren(const QModelIndex & parent) const
{
  std::cerr << "TreeModel::hasChildren" << std::endl;

  return rowCount(parent) > 0;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex & parent) const
{

  std::cerr << "TreeModel::index 1" << std::endl;
  if(!parent.isValid())
  {
    if(size_t(row) >= m_items.size())
    {
      std::cerr << "TreeModel::index 1.1" << std::endl;
      return QModelIndex();
    }

    QModelIndex idx = m_items[row]->modelIndex();
    if(!idx.isValid())
    {
      idx = createIndex(row, column, m_items[row]);
      m_items[row]->setModelIndex(idx);
    }
    std::cerr << "TreeModel::index 1.2" << std::endl;
    return idx;
  }

  std::cerr << "TreeModel::index 2" << std::endl;
  TreeItem * parentItem = (TreeItem *)parent.internalPointer();
  if(size_t(row) >= parentItem->numChildren())
  {
    std::cerr << "TreeModel::index 2.1" << std::endl;
    return QModelIndex();
  }

  std::cerr << "TreeModel::index 3" << std::endl;
  TreeItem * item = parentItem->child(row);
  std::cerr << "TreeModel::index 3.1" << std::endl;
  QModelIndex idx = item->modelIndex();
  std::cerr << "TreeModel::index 3.2" << std::endl;
  if(!idx.isValid())
  {
    std::cerr << "TreeModel::index 3.3" << std::endl;
    idx = createIndex(row, column, item);
    std::cerr << "TreeModel::index 3.4" << std::endl;
    item->setModelIndex(idx);
  }

  std::cerr << "TreeModel::index 3.5" << std::endl;
  return idx;
}

QModelIndex TreeModel::parent(const QModelIndex & index) const
{
  std::cerr << "TreeModel::parent" << std::endl;

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
  std::cerr << "TreeModel::rowCount" << std::endl;

  if(!parent.isValid())
    return ((TreeModel*)this)->numItems();

  TreeItem * parentItem = (TreeItem *)parent.internalPointer();
  return parentItem->numChildren();
}

void TreeModel::invalidateItem(TreeItem * item)
{
  std::cerr << "TreeModel::invalidateItem" << std::endl;
  emit dataChanged(item->modelIndex(), item->modelIndex());
}

void TreeModel::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
  std::cerr << "TreeModel::selectionChanged" << std::endl;
  for(size_t i=0;i<m_items.size();i++)
    m_items[i]->selectionChanged(selected, deselected);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  std::cerr << "TreeModel::flags" << std::endl;

  Qt::ItemFlags result = QAbstractItemModel::flags(index);
  TreeItem * item = (TreeItem *)index.internalPointer();
  if(item)
    result = result | item->flags();
  return result;
}

QMimeData * TreeModel::mimeData(const QModelIndexList &indexes) const
{
  std::cerr << "TreeModel::mimeData" << std::endl;

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
