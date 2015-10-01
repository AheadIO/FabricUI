// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "TreeViewWidget.h"
#include <QtGui/QHeaderView>
#include "TreeModel.h"
#include "TreeItemDelegate.h"
#include "TreeItem.h"

using namespace FabricUI::TreeView;

TreeViewWidget::TreeViewWidget(QWidget * parent, bool supportsDelegates)
:QTreeView(parent)
{
  setHeaderHidden(true);
  setSelectionMode(QAbstractItemView::ContiguousSelection);

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));
  connect(this, SIGNAL(expanded(const QModelIndex &)), this, SLOT(onExpanded(const QModelIndex &)));
  connect(this, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(onCollapsed(const QModelIndex &)));

  m_usesDelegate = supportsDelegates;
  if(supportsDelegates)
  {
    m_delegate = new TreeItemDelegate(this);
    setItemDelegate(m_delegate);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    header()->setHighlightSections(false);
    setMouseTracking(true);

    QObject::connect(this, SIGNAL(entered(const QModelIndex &)), this, SLOT(onEnteredIndex(const QModelIndex &)));
  }
  else
  {
    m_delegate = NULL;
  }
}

TreeViewWidget::~TreeViewWidget()
{
}

QString TreeViewWidget::state() const
{
  QString result;

  QAbstractItemModel * abstractModel = model();
  if(!abstractModel)
    return result;
  
  TreeModel * model = (TreeModel *)abstractModel;
  std::vector<TreeItem*> items;
  for(unsigned int i=0;i<model->numItems();i++)
    items.push_back(model->item(i));

  for(unsigned int i=0;i<items.size();i++)
  {
    if(items[i]->collapsed())
      continue;

    result += QString( (items[i]->path()+";").c_str() );

    for(unsigned int j=0;j<items[i]->numChildren();j++)
      items.push_back(items[i]->child(j));
  }

  return result;
}

void TreeViewWidget::setState(QString s)
{
  if(s.isEmpty())
    return;

  QAbstractItemModel * abstractModel = model();
  if(!abstractModel)
    return;
  TreeModel * model = (TreeModel *)abstractModel;
  QStringList itemPaths = s.split(';');

  for(int i=0;i<itemPaths.length();i++)
  {
    if(itemPaths[i].isEmpty())
      continue;

    std::string itemPath = itemPaths[i].toUtf8().constData();
    TreeItem * item = model->item( itemPath );
    if(!item)
      continue;

    item->numChildren();
    setExpanded(item->modelIndex(model), true);
  }

  update();
}

void TreeViewWidget::tabForwards()
{
  QAbstractItemModel * abstractModel = model();
  if(!abstractModel)
    return;

  TreeModel * model = (TreeModel *)abstractModel;
  TreeItem * current = model->item(currentIndex());
  if(!current)
    return;

  TreeItem * sibling = current->sibling();
  if(sibling == current)
    return;

  onEnteredIndex(sibling->modelIndex(model));
}

void TreeViewWidget::onCustomContextMenuRequested(const QPoint & point)
{
  QModelIndex index = indexAt(point);
  if (index.isValid())
  {
    TreeItem * item = (TreeItem *)index.internalPointer();
    emit customContextMenuRequested(mapToGlobal(point), item);
    showCustomContextMenu(mapToGlobal(point), item);
  }    
}

void TreeViewWidget::setFactory(TreeEditorFactory * factory)
{
  if(m_delegate)
    m_delegate->setFactory(factory);
}

void TreeViewWidget::onExpanded(const QModelIndex & index)
{
  if(!index.isValid())
    return;

  TreeItem * item = (TreeItem *)index.internalPointer();
  if(!item)
    return;

  item->setExpanded(true);
}

void TreeViewWidget::onCollapsed(const QModelIndex & index)
{
  if(!index.isValid())
    return;

  TreeItem * item = (TreeItem *)index.internalPointer();
  if(!item)
    return;
  item->setExpanded(false);
}

void TreeViewWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
  QAbstractItemModel * abstractModel = model();
  if(abstractModel) {
    TreeModel * model = (TreeModel *)abstractModel;
    model->selectionChanged(selected, deselected);
  }

  QTreeView::selectionChanged(selected, deselected);

  if(abstractModel)
    emit itemSelectionChanged();
}

void TreeViewWidget::onEnteredIndex(const QModelIndex & index)
{
  if(index.isValid() && m_usesDelegate)
  {
    setCurrentIndex(index);
    edit(index, QAbstractItemView::AllEditTriggers, NULL);

    QAbstractItemModel * abstractModel = model();
    if(abstractModel)
    {
      TreeModel * model = (TreeModel *)abstractModel;
      TreeItem * item = model->item(index);
      WidgetTreeItem * widgetItem = dynamic_cast<WidgetTreeItem *>(item);
      if(widgetItem)
      {
        if(widgetItem->editor())
          widgetItem->editor()->setFocusToFirstInput();
      }
    }
  }
}
