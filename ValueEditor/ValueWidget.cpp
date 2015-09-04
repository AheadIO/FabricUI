// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ValueWidget.h"
#include "ValueItem.h"
#include "ValueEditorWidget.h"
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

ValueWidget::ValueWidget(QString label, QWidget * parent, bool requiresEditability)
: TreeEditorWidget(parent)
{
  setStyle(QApplication::style());  

  setMinimumHeight(24);
  setBackgroundRole(QPalette::Window);
  setAutoFillBackground(true);

  QHBoxLayout * layout = new QHBoxLayout();
  setLayout(layout);

  setContentsMargins(4, 2, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  m_label = new QLabel(label, this);
  if(requiresEditability)
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  m_label->setMinimumWidth(EditorConfig().minLabelWidth);
  layout->addWidget(m_label);
}

ValueWidget::~ValueWidget()
{
}

FabricCore::RTVal ValueWidget::value() const
{
  return m_value;
}

void ValueWidget::setValue(FabricCore::RTVal v)
{
  m_value = v;
  emit dataChanged();
}

ValueItem * ValueWidget::valueItem()
{
  return (ValueItem*)item();
}

TreeEditorWidget * ValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  ValueWidget * widget = new ValueWidget( item->label().c_str(), parent );
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool ValueWidget::canDisplay(WidgetTreeItem * item)
{
  QString typeName = ((ValueItem*)item)->valueTypeName();
  return item->type() == "ValueItem" && typeName == ""; // only accept empty RTVals
}

ValueEditorWidget * ValueWidget::mainEditorWidget()
{
  QObject * p = parent();
  while(p != NULL)
  {
    ValueEditorWidget * result = dynamic_cast<ValueEditorWidget *>(p);
    if(result != NULL)
      return result;
    QObject * p2 = p->parent();
    if(p2 == NULL)
      return NULL;
    if(p2 == p)
      return NULL;
    p = p2;
  }
  return NULL;
}
