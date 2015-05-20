// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ValueWidget.h"
#include "ValueItem.h"
#include "ValueEditorWidget.h"
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

ValueWidget::ValueWidget(QString label, QWidget * parent, bool labelRightAligned)
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
  if(labelRightAligned)
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
  ValueWidget * widget = new ValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool ValueWidget::canDisplay(WidgetTreeItem * item)
{
  return item->type() == "ValueItem";
}
