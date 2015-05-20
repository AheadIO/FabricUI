// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "TreeEditorFactory.h"

using namespace FabricUI::TreeView;

TreeEditorFactory::TreeEditorFactory()
{
}

TreeEditorFactory::~TreeEditorFactory()
{
}

bool TreeEditorFactory::canDisplay(WidgetTreeItem * item) const
{
  for(int i=m_editors.size()-1;i>=0;i--)
  {
    if((*m_editors[i].canDisplay)(item))
    {
      return true;
    }
  }
  return false;
}

TreeEditorWidget * TreeEditorFactory::createEditor(QWidget * parent, WidgetTreeItem * item) const
{
  for(int i=m_editors.size()-1;i>=0;i--)
  {
    if((*m_editors[i].canDisplay)(item))
    {
      TreeEditorWidget * widget = (*m_editors[i].creator)(parent, item);
      item->setEditor(widget);
      return widget;
    }
  }
  return NULL;
}

void TreeEditorFactory::registerEditor(TreeEditorWidget::CreatorFunc creator, TreeEditorWidget::CanDisplayFunc canDisplay)
{
  Editor editor;
  editor.creator = creator;
  editor.canDisplay = canDisplay;
  m_editors.push_back(editor);
}
