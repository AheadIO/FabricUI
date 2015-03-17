// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "TreeEditorWidget.h"
#include "WidgetTreeItem.h"

using namespace FabricUI::TreeView;

TreeEditorWidget::TreeEditorWidget(QWidget * parent)
: QWidget(parent)
{
  m_source = NULL;
  setFocusPolicy(Qt::ClickFocus);
}

void TreeEditorWidget::changeData()
{
  emit dataChanged();
}

void TreeEditorWidget::setItem(WidgetTreeItem * item)
{
  m_item = item;
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

