// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "BooleanValueWidget.h"
#include "ValueItem.h"
#include "ValueEditorEventFilters.h"

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

BooleanValueWidget::BooleanValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_checkBox = new QCheckBox(this);
  m_checkBox->setFocusPolicy(Qt::StrongFocus);

  hbox->addWidget(m_checkBox);

  m_checkBox->installEventFilter(new BackspaceDeleteEventFilter(this));

  QObject::connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(onValueChangedInCheckBox()));
}

BooleanValueWidget::~BooleanValueWidget()
{
}

void BooleanValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);
  bool b = v.getBoolean();
  m_checkBox->setCheckState(b ? Qt::Checked : Qt::Unchecked);
}

void BooleanValueWidget::setEnabled(bool state)
{
  m_checkBox->setEnabled(state);
}

void BooleanValueWidget::onValueChangedInCheckBox()
{
  bool b = m_checkBox->checkState() == Qt::Checked;
  ValueWidget::setValue(FabricCore::RTVal::ConstructBoolean(*((ValueItem*)item())->client(), b));
}

TreeEditorWidget * BooleanValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  BooleanValueWidget * widget = new BooleanValueWidget(item->label().c_str(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool BooleanValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(item->type() != "ValueItem")
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "Boolean";
}
