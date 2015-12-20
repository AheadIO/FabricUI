// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ValueEditorEventFilters.h"

#include <QtCore/Qt>
#include <QtGui/QKeyEvent>

using namespace FabricUI::ValueEditor;

EventFilter::EventFilter(ValueWidget * itemWidget)
: QObject(itemWidget)
, m_itemWidget(itemWidget)
{
}

bool EventFilter::eventFilter(QObject *object, QEvent *event)
{
  return false;
}

TabEventFilter::TabEventFilter(ValueWidget * itemWidget)
: EventFilter(itemWidget)
{
}

bool TabEventFilter::eventFilter(QObject *object, QEvent *event)
{
  if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if(keyEvent->key() == Qt::Key_Tab)
    {
      if(event->type() == QEvent::KeyPress)
      {
        ValueEditorWidget * editor = m_itemWidget->mainEditorWidget();
        if(editor)
        {
          editor->treeView()->tabForwards();
          return true;
        }
      }
    }
  }

  return EventFilter::eventFilter(object, event);
}

BackspaceDeleteEventFilter::BackspaceDeleteEventFilter(ValueWidget * itemWidget)
: EventFilter(itemWidget)
{
}

bool BackspaceDeleteEventFilter::eventFilter(QObject *object, QEvent *event)
{
  if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if(keyEvent->key() == Qt::Key_Delete)
      return true;
    if(keyEvent->key() == Qt::Key_Backspace)
      return true;
  }

  return EventFilter::eventFilter(object, event);
}
