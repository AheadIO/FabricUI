// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>

#include "WidgetTreeItem.h"
#include "TreeEditorFactory.h"
#include <iostream>
using namespace FabricUI::TreeView;

WidgetTreeItem::WidgetTreeItem(
  FTL::StrRef name,
  TreeEditorFactory * factory,
  FTL::StrRef label,
  QWidget * parent
  )
  : TreeItem(name, label)
  , m_parent(parent)
{
  //std::cerr << "WidgetTreeItem::WidgetTreeItem" << std::endl;
  m_editor = NULL;
  m_factory = factory;
  m_pixmap = NULL;
  m_needsUpdate = false;
}

WidgetTreeItem::~WidgetTreeItem()
{
  //std::cerr << "WidgetTreeItem::~WidgetTreeItem" << std::endl;
  if(m_pixmap)
    delete(m_pixmap);
}

TreeItemType WidgetTreeItem::itemType() const
{
  return TreeItemType_Widget;
}

void WidgetTreeItem::onEditorDestroyed(QObject * obj)
{  
  //std::cerr << "WidgetTreeItem::onEditorDestroyed" << std::endl;
  m_editor = NULL;
}

QSize WidgetTreeItem::sizeHint(const QStyleOptionViewItem & option) const
{
  //std::cerr << "WidgetTreeItem::sizeHint" << std::endl;
  if(!m_minimumSize.isValid())
  {
    if(m_editor)
    {
      m_minimumSize = m_editor->minimumSize();
    }
    else
    {
      TreeEditorWidget * editor = createTempEditor();
      if(editor)
      {
        m_minimumSize = editor->minimumSize();
        delete(editor);
      }
    }
  }
  return m_minimumSize;
}

void WidgetTreeItem::paint(QPainter * painter, const QStyleOptionViewItem & option) const
{
  //std::cerr << "WidgetTreeItem::paint" << std::endl;
  QSize size(option.rect.width(), option.rect.height());

  bool needsUpdate = m_needsUpdate;
  
  if(!m_pixmap)
  {
    m_pixmap = new QPixmap(size.width(), size.height());
    needsUpdate = true;
  }
  else if(m_pixmap->width() != size.width() || m_pixmap->height() != size.height())
  {
    if(m_pixmap)
      delete(m_pixmap);
    m_pixmap = new QPixmap(size.width(), size.height());
    needsUpdate = true;
  }

  bool createdEditor = false;
  if(m_editor == NULL && needsUpdate)
  {
    m_editor = createTempEditor();
    createdEditor = m_editor != NULL;
  }

  if(m_editor)
  {
    m_minimumSize = m_editor->minimumSize();

    if(m_editor->size() != size)
    {
      m_editor->resize(size);
      m_editor->update();
    }

    updatePixmap();
  }

  if(createdEditor)
  {
    delete(m_editor);
    m_editor = NULL;
  }

  if(!m_editor)
    painter->drawPixmap(option.rect.topLeft(), *m_pixmap);
}

TreeEditorWidget * WidgetTreeItem::editor()
{
  //std::cerr << "WidgetTreeItem::editor" << std::endl;
  return m_editor;
}

void WidgetTreeItem::setEditor(TreeEditorWidget * editor)
{
  //std::cerr << "WidgetTreeItem::setEditor" << std::endl;
  m_editor = editor;
  if(m_editor)
  {
    QObject::connect(m_editor, SIGNAL(destroyed(QObject *)), this, SLOT(onEditorDestroyed(QObject *)));
    QObject::connect(m_editor, SIGNAL(dataChanged()), this, SLOT(updatePixmap()));
    QObject::connect(m_editor, SIGNAL(dataChanged()), this, SLOT(updateFromUI()));
  }
}

void WidgetTreeItem::setSelected(bool state)
{
  //std::cerr << "WidgetTreeItem::setSelected" << std::endl;
  bool prev = selected();
  TreeItem::setSelected(state);
  if(prev != state)
  {
    if(m_editor)
      emit m_editor->selected(state);
  }
}

void WidgetTreeItem::setExpanded(bool state)
{
  //std::cerr << "WidgetTreeItem::setExpanded" << std::endl;
  bool prev = expanded();
  TreeItem::setExpanded(state);
  if(prev != state)
  {
    if(m_editor)
      emit m_editor->expanded(state);
    else
    {
      TreeEditorWidget * editor = createTempEditor();
      if(editor)
      {
        emit editor->expanded(state);
        updatePixmap();
        delete(editor);
      }
    }
  }
}

TreeEditorWidget * WidgetTreeItem::createTempEditor() const
{
  //std::cerr << "WidgetTreeItem::createTempEditor" << std::endl;
  QStyleOptionViewItem option;
  return m_factory->createEditor(m_parent, (WidgetTreeItem*)this);
}

void WidgetTreeItem::updateFromUI()
{
  //std::cerr << "WidgetTreeItem::updateFromUI" << std::endl;
  onUIChanged();
}

void WidgetTreeItem::updatePixmap() const
{
  //std::cerr << "WidgetTreeItem::updatePixmap" << std::endl;
  if(m_editor && m_pixmap)
  {
    if(m_editor->size() == m_pixmap->size())
    {
      m_editor->render(m_pixmap);
      m_needsUpdate = false;
    }
  }
}

void WidgetTreeItem::onDataChanged()
{
  //std::cerr << "WidgetTreeItem::onDataChanged" << std::endl;
  if(m_editor)
  {
    m_editor->changeData();
    updatePixmap();
  }
}
