// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "TreeItem.h"

using namespace FabricUI::TreeView;

TreeItem::TreeItem(
  FTL::StrRef name,
  FTL::StrRef label
  )
  : m_name( name )
  , m_label( !label.empty()? label: name )
{
  m_parent = NULL;
  m_model = NULL;
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

std::string TreeItem::path() const
{
  if(m_parent == NULL)
    return m_name;
  return m_parent->path() + "." + m_name;
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

QModelIndex TreeItem::modelIndex(TreeModel * model) const
{
  // try to get it by walking up
  if(!m_modelIndex.isValid() && model)
  {
    QModelIndex parentIndex;
    if(m_parent)
      parentIndex = m_parent->modelIndex(model);
    m_modelIndex = model->index(m_index, 0, parentIndex);
  }
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
  bool prevState = m_selected;
  m_selected = state;
  if(m_selected != prevState)
  {
    if(m_model)
    {
      if(m_selected)
        emit m_model->itemSelected(this);
      else
        emit m_model->itemDeselected(this);
    }
  }
}

void TreeItem::setExpanded(bool state)
{
  bool prevState = m_expanded;
  m_expanded = state;
  if(m_expanded != prevState)
  {
    if(m_model)
    {
      if(m_expanded)
        emit m_model->itemExpanded(this);
      else
        emit m_model->itemCollapsed(this);
    }
  }
}

unsigned int TreeItem::numChildren()
{
  return m_children.size();
}

void TreeItem::addChild(TreeItem * childToAdd)
{
  childToAdd->setParent(this);
  childToAdd->setModel(m_model);
  childToAdd->setIndex(m_children.size());
  m_children.push_back(childToAdd);
  emit childAdded(childToAdd);
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
  emit childRemoved(childToRemove);

  return true;
}

TreeItem * TreeItem::child(unsigned int i)
{
  return m_children[i];
}

TreeItem * TreeItem::child( FTL::StrRef path )
{
  FTL::StrRef::Split split = path.split('.');
  if ( split.first.empty() )
    return this;

  for ( size_t i = 0; i < m_children.size(); ++i )
  {
    if ( m_children[i]->name() == split.first )
    {
      if ( split.second.empty() )
        return m_children[i];
      else
        m_children[i]->child( split.second );
    }
  }

  return NULL;
}

TreeItem * TreeItem::child( QModelIndex index )
{
  for ( size_t i = 0; i < m_children.size(); ++i )
  {
    TreeItem * candidate = m_children[i];
    if ( candidate->modelIndex() == index )
      return candidate;
    candidate = candidate->child( index );
    if(candidate)
      return candidate;
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

TreeModel * TreeItem::model()
{
  return m_model;
}

TreeItem * TreeItem::sibling(bool next)
{
  if(m_parent)
  {
    std::vector<TreeItem*> & children = m_parent->m_children;
    for(size_t i=0;i<children.size();i++)
    {
      if(children[i] == this)
      {
        if(next)
          return children[(i+1) % children.size()];
        else
          return children[(i+children.size()-1) % children.size()];
      }
    }
  }
  else if(m_model)
  {
    std::vector<TreeItem*> items = m_model->m_items;
    for(size_t i=0;i<items.size();i++)
    {
      if(items[i] == this)
      {
        if(next)
          return items[(i+1) % items.size()];
        else
          return items[(i+items.size()-1) % items.size()];
      }
    }
  }
  return NULL;
}


void TreeItem::setModel(TreeModel * model)
{
  m_model = model;
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
      return QString( label().c_str() );
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
