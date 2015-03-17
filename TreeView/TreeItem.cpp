// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "TreeItem.h"

using namespace FabricUI::TreeView;

TreeItem::TreeItem(QString name, QString type, QString label)
{
  m_name = name;
  m_label = label;
  if(m_label.length() == 0)
    m_label = m_name;
  m_type = type;
  m_parent = NULL;
  m_index = 0;
  m_selected = false;
  m_expanded = false;
  m_editable = true;
  m_backgroundColor = QColor();
  m_foregroundColor = QColor();
}

TreeItem::~TreeItem()
{
  for(size_t i=0;i<m_children.size();i++)
    delete(m_children[i]);
}

TreeItemType TreeItem::itemType() const
{
  return TreeItemType_Default;
}

bool TreeItem::editable() const
{
  return m_editable;
}

void TreeItem::setEditable(bool state)
{
  m_editable = state;  
}

QString TreeItem::name() const
{
  return m_name;
}

QString TreeItem::label() const
{
  return m_label;
}

QString TreeItem::path() const
{
  if(m_parent == NULL)
    return m_name;
  return m_parent->path() + "." + m_name;
}

QString TreeItem::type() const
{
  return m_type;
}

QColor TreeItem::foregroundColor() const
{
  return m_foregroundColor;
}

void TreeItem::setForegroundColor(QColor color)
{
  m_foregroundColor = color;
}

QColor TreeItem::backgroundColor() const
{
  return m_backgroundColor;
}

void TreeItem::setBackgroundColor(QColor color)
{
  m_backgroundColor = color;
}

QPixmap TreeItem::pixmap() const
{
  return m_pixmap;
}

void TreeItem::setPixmap(QPixmap pm)
{
  m_pixmap = pm;
}

unsigned int TreeItem::index() const
{
  return m_index;
}

void TreeItem::setIndex(unsigned int i)
{
  m_index = i;
}

QModelIndex TreeItem::modelIndex() const
{
  return m_modelIndex;
}

bool TreeItem::selected() const
{
  return m_selected;
}

bool TreeItem::expanded() const
{
  return m_expanded;
}

bool TreeItem::collapsed() const
{
  return !expanded();
}

void TreeItem::setSelected(bool state)
{
  m_selected = state;
}

void TreeItem::setExpanded(bool state)
{
  m_expanded = state;
}

unsigned int TreeItem::numChildren()
{
  return m_children.size();
}

void TreeItem::addChild(TreeItem * childToAdd)
{
  childToAdd->setParent(this);
  childToAdd->setIndex(m_children.size());
  m_children.push_back(childToAdd);
}

bool TreeItem::removeChild(TreeItem * childToRemove)
{
  unsigned int index = m_children.size();
  for(unsigned int i=0;i<m_children.size();i++)
  {
    if(m_children[i] == childToRemove)
    {
      index = i;
      break;
    }
  }

  if(index == m_children.size())
    return false;

  m_children.erase(m_children.begin() + index);
  return true;
}

TreeItem * TreeItem::child(unsigned int i)
{
  return m_children[i];
}

TreeItem * TreeItem::child(QString path)
{
  int pos = path.indexOf('.');
  QString left = path;
  QString right;
  if(pos > -1)
  {
    left = path.left(pos);
    right = path.right(path.length()-pos-1);
  }

  for(size_t i=0;i<m_children.size();i++)
  {
    if(m_children[i]->name() == left)
    {
      if(right.length() == 0)
        return m_children[i];
      return m_children[i]->child(right);
    }
  }
  return NULL;
}

TreeItem * TreeItem::parent()
{
  return m_parent;
}

void TreeItem::setParent(TreeItem * item)
{
  m_parent = item;
}

void TreeItem::setModelIndex(QModelIndex modelIndex)
{
  m_modelIndex = modelIndex;
}

void TreeItem::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
  if(selected.contains(m_modelIndex))
    setSelected(true);
  if(deselected.contains(m_modelIndex))
    setSelected(false);

  for(size_t i=0;i<m_children.size();i++)
    m_children[i]->selectionChanged(selected, deselected);
}

Qt::ItemFlags TreeItem::flags()
{
  Qt::ItemFlags flags = Qt::NoItemFlags;
  if(editable())
  {
    flags = flags | Qt::ItemIsEditable;
  }
  return flags;
}

QVariant TreeItem::data(int role)
{
  switch(role)
  {
    case Qt::DisplayRole:
    {
      return label();
    }
    case Qt::DecorationRole:
    {
      if(m_pixmap.width() > 0)
        return m_pixmap;
    }
    case Qt::ForegroundRole:
    {
      if(foregroundColor().isValid())
        return QBrush(foregroundColor());
    }
    case Qt::BackgroundRole:
    {
      if(backgroundColor().isValid())
        return QBrush(backgroundColor());
    }
    default:
    {
      return QVariant();
    }
  }
}

QString TreeItem::mimeDataAsText()
{
  return "null";
}
