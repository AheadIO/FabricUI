// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "VariablePathValueWidget.h"
#include "VariablePathValueItem.h"
#include "Dialogs/DFGVariablePathLineEdit.h"
#include <FabricUI/ValueEditor/ValueItem.h>
#include <FabricUI/ValueEditor/ValueWidget.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

VariablePathValueWidget::VariablePathValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  m_changingValue = false;
  m_lineEdit = NULL;
}

VariablePathValueWidget::~VariablePathValueWidget()
{
}

void VariablePathValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);
  QString s = v.getStringCString();

  if(m_lineEdit == NULL)
  {
    QHBoxLayout * hbox = (QHBoxLayout *)layout();

    VariablePathValueItem * varPathItem = (VariablePathValueItem*)item();
    m_lineEdit = new DFGVariablePathLineEdit(this, varPathItem->getDFGBinding(), varPathItem->getDFGExecPath());
    hbox->addWidget(m_lineEdit);
    QObject::connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  }

  if(m_lineEdit)
    m_lineEdit->setText(s);
}

void VariablePathValueWidget::setEnabled(bool state)
{
  if(m_lineEdit)
    m_lineEdit->setEnabled(state);
}

void VariablePathValueWidget::onValueChangedInLineEdit()
{
  if(m_changingValue)
    return;
  m_changingValue = true;

  QString s = m_lineEdit->text();
  m_value = FabricCore::RTVal::ConstructString(*((ValueEditor::ValueItem*)item())->client(), s.toUtf8().constData());
  emit dataChanged();

  m_changingValue = false;
}

TreeView::TreeEditorWidget * VariablePathValueWidget::creator(QWidget * parent, TreeView::WidgetTreeItem * item)
{
  VariablePathValueWidget * widget = new VariablePathValueWidget(item->label().c_str(), parent);
  widget->setItem(item);
  widget->setValue(((ValueEditor::ValueItem*)item)->value());
  return widget;
}

bool VariablePathValueWidget::canDisplay(TreeView::WidgetTreeItem * item)
{
  return item->type() == "VariablePath";
}
