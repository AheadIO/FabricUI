// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout> 

#include "ValueEditorWidget.h"
#include "ValueWidgetFactory.h"
#include "ValueItem.h"

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

ValueEditorWidget::ValueEditorWidget(
  FabricCore::Client client,
  const EditorConfig & config
  )
  : m_config(config)
{
  setMinimumHeight(24);
  setBackgroundRole(QPalette::Window);
  setAutoFillBackground(true);

  m_client = client;

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  m_treeView = new TreeView::TreeViewWidget(this, true /* delegate support */);
  m_treeModel = new TreeView::TreeModel(this);
  m_factory = new ValueWidgetFactory();
  m_treeView->setModel(m_treeModel);
  m_treeView->setFactory(m_factory);

  layout->addWidget(m_treeView);
}

ValueEditorWidget::~ValueEditorWidget()
{
}

EditorConfig & ValueEditorWidget::config()
{
  return m_config;
}

ValueItem * ValueEditorWidget::addValue(QString path, FabricCore::RTVal value, QString label, bool enabled)
{
  int pos = path.lastIndexOf('.');
  QString left = path;
  QString right;
  if(pos > -1)
  {
    left = path.left(pos);
    right = path.right(path.length()-pos-1);
  }

  if(right.length() == 0)
  {
    TreeItem * item = m_treeModel->item(left);
    if(item != NULL)
      return NULL;
    ValueItem * newItem = new ValueItem(left, m_factory, &m_client, m_treeView, value, label, enabled);
    if(value.isValid())
    {
      if(!m_factory->canDisplay(newItem))
      {
        delete(newItem);
        return NULL;
      }
    }
    QObject::connect(newItem, SIGNAL(beginInteraction(ValueItem*)), this, SIGNAL(beginInteraction(ValueItem*)));
    QObject::connect(newItem, SIGNAL(valueChanged(ValueItem*)), this, SIGNAL(valueChanged(ValueItem*)));
    QObject::connect(newItem, SIGNAL(endInteraction(ValueItem*)), this, SIGNAL(endInteraction(ValueItem*)));
    m_treeModel->addItem(newItem);
    return newItem;
  }
  else
  {
    TreeItem * item = m_treeModel->item(left);
    if(item == NULL)
      return NULL;
    ValueItem * newItem = new ValueItem(right, m_factory, &m_client, m_treeView, value, label, enabled);
    if(value.isValid())
    {
      if(!m_factory->canDisplay(newItem))
      {
        delete(newItem);
        return NULL;
      }
    }
    QObject::connect(newItem, SIGNAL(beginInteraction(ValueItem*)), this, SIGNAL(beginInteraction(ValueItem*)));
    QObject::connect(newItem, SIGNAL(valueChanged(ValueItem*)), this, SIGNAL(valueChanged(ValueItem*)));
    QObject::connect(newItem, SIGNAL(endInteraction(ValueItem*)), this, SIGNAL(endInteraction(ValueItem*)));
    item->addChild(newItem);
    return newItem;
  }

  return NULL;
}

ValueItem * ValueEditorWidget::addValue(QString path, ValueItem * newItem, bool enabled)
{
  int pos = path.lastIndexOf('.');
  QString left = path;
  QString right;
  if(pos > -1)
  {
    left = path.left(pos);
    right = path.right(path.length()-pos-1);
  }

  if(right.length() == 0)
  {
    TreeItem * item = m_treeModel->item(left);
    if(item != NULL)
      return NULL;
    if(!m_factory->canDisplay(newItem))
      return NULL;
    QObject::connect(newItem, SIGNAL(beginInteraction(ValueItem*)), this, SIGNAL(beginInteraction(ValueItem*)));
    QObject::connect(newItem, SIGNAL(valueChanged(ValueItem*)), this, SIGNAL(valueChanged(ValueItem*)));
    QObject::connect(newItem, SIGNAL(endInteraction(ValueItem*)), this, SIGNAL(endInteraction(ValueItem*)));
    m_treeModel->addItem(newItem);
    return newItem;
  }
  else
  {
    TreeItem * item = m_treeModel->item(left);
    if(item == NULL)
      return NULL;
    if(!m_factory->canDisplay(newItem))
      return NULL;
    QObject::connect(newItem, SIGNAL(beginInteraction(ValueItem*)), this, SIGNAL(beginInteraction(ValueItem*)));
    QObject::connect(newItem, SIGNAL(valueChanged(ValueItem*)), this, SIGNAL(valueChanged(ValueItem*)));
    QObject::connect(newItem, SIGNAL(endInteraction(ValueItem*)), this, SIGNAL(endInteraction(ValueItem*)));
    item->addChild(newItem);
    return newItem;
  }

  return NULL;
}


bool ValueEditorWidget::removeValue(QString path)
{
  TreeItem * item = m_treeModel->item(path);
  if(item == NULL)
    return false;

  TreeItem * parent = item->parent();
  if(parent)
    parent->removeChild(item);
  else
    m_treeModel->removeItem(item);
  delete(item);
  return true;
}

FabricCore::RTVal ValueEditorWidget::getValue(QString path)
{
  ValueItem * item = (ValueItem *)m_treeModel->item(path);
  if(item == NULL)
    return FabricCore::RTVal();
  return item->value();
}

void ValueEditorWidget::clear()
{
  blockSignals(true);
  m_treeModel->clear();
  blockSignals(false);
}
