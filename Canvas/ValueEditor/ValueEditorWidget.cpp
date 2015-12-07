// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout> 
#include <QtCore/Qt>

#include "ValueEditorWidget.h"
#include "ValueWidgetFactory.h"
#include "ValueItem.h"

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

ValueEditorWidget::ValueEditorWidget(
  FabricCore::Client client,
  const EditorConfig & config
  )
  : m_config( config )
  , m_updatingOutputs( false )
{
  setMinimumHeight(24);
  setBackgroundRole(QPalette::Window);
  setAutoFillBackground(true);

  m_client = client;

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  m_treeView  = new TreeView::TreeViewWidget(this, true /* delegate support */);
  m_treeModel = new TreeView::TreeModel(this);
  m_factory   = new ValueWidgetFactory();
  m_treeView->setModel(m_treeModel);
  m_treeView->setFactory(m_factory);
  m_treeView->setIndentation(0);  /*  set the indentation to 0 pixels to avoid wasting space and as a fix for FE-5120 */

  layout->addWidget(m_treeView);
}

ValueEditorWidget::~ValueEditorWidget()
{
}

EditorConfig & ValueEditorWidget::config()
{
  return m_config;
}

ValueItem * ValueEditorWidget::addValue(
  FTL::StrRef path,
  FabricCore::RTVal value,
  FTL::StrRef label,
  bool enabled,
  bool parentToRoot
  )
{
  FTL::StrRef left = path;
  FTL::StrRef right;

  if ( !parentToRoot )
  {
    FTL::StrRef::Split split = path.rsplit('.');
    if ( !split.first.empty() )
    {
      left = split.first;
      right = split.second;
    }
  }

  if ( right.empty() )
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
    QObject::connect(
      newItem, SIGNAL(valueItemDelta(ValueItem*)),
      this, SLOT(onValueItemDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionEnter(ValueItem*)),
      this, SIGNAL(valueItemInteractionEnter(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionDelta(ValueItem*)),
      this, SIGNAL(valueItemInteractionDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionLeave(ValueItem*)),
      this, SIGNAL(valueItemInteractionLeave(ValueItem*))
      );
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
    QObject::connect(
      newItem, SIGNAL(valueItemDelta(ValueItem*)),
      this, SIGNAL(valueItemDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionEnter(ValueItem*)),
      this, SIGNAL(valueItemInteractionEnter(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionDelta(ValueItem*)),
      this, SIGNAL(valueItemInteractionDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionLeave(ValueItem*)),
      this, SIGNAL(valueItemInteractionLeave(ValueItem*))
      );
    item->addChild(newItem);
    return newItem;
  }

  return NULL;
}

ValueItem * ValueEditorWidget::addValue(
  FTL::StrRef path,
  ValueItem * newItem,
  bool enabled
  )
{
  FTL::StrRef::Split split = path.rsplit('.');
  FTL::StrRef left = split.first;
  FTL::StrRef right = split.second;

  if ( right.empty() )
  {
    TreeItem * item = m_treeModel->item(left);
    if(item != NULL)
      return NULL;
    if(!m_factory->canDisplay(newItem))
      return NULL;
    QObject::connect(
      newItem, SIGNAL(valueItemDelta(ValueItem*)),
      this, SIGNAL(valueItemDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionEnter(ValueItem*)),
      this, SIGNAL(valueItemInteractionEnter(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionDelta(ValueItem*)),
      this, SIGNAL(valueItemInteractionDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionLeave(ValueItem*)),
      this, SIGNAL(valueItemInteractionLeave(ValueItem*))
      );
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
    QObject::connect(
      newItem, SIGNAL(valueItemDelta(ValueItem*)),
      this, SIGNAL(valueItemDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionEnter(ValueItem*)),
      this, SIGNAL(valueItemInteractionEnter(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionDelta(ValueItem*)),
      this, SIGNAL(valueItemInteractionDelta(ValueItem*))
      );
    QObject::connect(
      newItem, SIGNAL(valueItemInteractionLeave(ValueItem*)),
      this, SIGNAL(valueItemInteractionLeave(ValueItem*))
      );
    item->addChild(newItem);
    return newItem;
  }

  return NULL;
}

bool ValueEditorWidget::removeValue( FTL::StrRef path )
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

FabricCore::RTVal ValueEditorWidget::getValue( FTL::StrRef path )
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

void ValueEditorWidget::keyPressEvent(QKeyEvent * event)
{
  if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
  {
    event->accept();
    return;
  }
  return QWidget::keyPressEvent(event);
}

void ValueEditorWidget::onValueItemDelta( ValueItem *valueItem )
{
  if ( !m_updatingOutputs )
    emit valueItemDelta( valueItem );
}
