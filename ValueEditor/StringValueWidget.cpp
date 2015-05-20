// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "StringValueWidget.h"
#include "ValueItem.h"

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

StringValueWidget::StringValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_lineEdit = new QLineEdit(this);
  hbox->addWidget(m_lineEdit);

  QObject::connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
}

StringValueWidget::~StringValueWidget()
{
}

void StringValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);
  QString s = v.getStringCString();
  m_lineEdit->setText(s);
}

void StringValueWidget::setEnabled(bool state)
{
  m_lineEdit->setEnabled(state);
}

void StringValueWidget::onValueChangedInLineEdit()
{
  QString s = m_lineEdit->text();
  m_value = FabricCore::RTVal::ConstructString(*((ValueItem*)item())->client(), s.toUtf8().constData());
  emit dataChanged();
}

TreeEditorWidget * StringValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  StringValueWidget * widget = new StringValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool StringValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(!ValueWidget::canDisplay(item))
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "String";
}
