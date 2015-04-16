// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "TreeEditorWidget.h"
#include "WidgetTreeItem.h"

using namespace FabricUI::TreeView;

TreeEditorWidget::TreeEditorWidget(QWidget * parent)
: QWidget(parent)
{
  m_source = NULL;
  m_changingData = false;
  setFocusPolicy(Qt::ClickFocus);
}

void TreeEditorWidget::changeData()
{
  if(m_changingData)
    return;
  m_changingData = true;
  emit dataChanged();
  m_changingData = false;
}

void TreeEditorWidget::setItem(WidgetTreeItem * item)
{
  if(m_item == item)
    return;
  m_item = item;
  emit itemChanged();
}

WidgetTreeItem * TreeEditorWidget::item()
{
  return m_item;
}

bool TreeEditorWidget::itemSelected() const
{
  return m_item->selected();
}

bool TreeEditorWidget::itemExpanded() const
{
  return m_item->expanded();
}

bool TreeEditorWidget::itemCollapsed() const
{
  return m_item->collapsed();
}

