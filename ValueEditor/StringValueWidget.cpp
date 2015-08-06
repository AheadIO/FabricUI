// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "StringValueWidget.h"
#include "ValueItem.h"

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

StringValueWidget::StringValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  m_changingValue = false;
  m_lineEdit = NULL;
  m_comboBox = NULL;
}

StringValueWidget::~StringValueWidget()
{
}

void StringValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);
  QString s = v.getStringCString();

  if(m_comboBox == NULL && m_lineEdit == NULL)
  {
    QHBoxLayout * hbox = (QHBoxLayout *)layout();

    QString uiCombo = valueItem()->getMetaData("uiCombo");
    if(uiCombo.length() > 0)
    {
      if(uiCombo[0] == '(')
        uiCombo = uiCombo.mid(1);
      if(uiCombo[uiCombo.length()-1] == ')')
        uiCombo = uiCombo.left(uiCombo.length()-1);

      QStringList parts = uiCombo.split(',');
      if(parts.length() > 1)
      {
        m_comboBox = new QComboBox(this);
        hbox->addWidget(m_comboBox);

        for(int i=0;i<parts.size();i++)
        {
          QString part = parts[i].trimmed();
          part = part.remove('"');
          part = part.remove('\'');
          m_comboBox->addItem(part);
        }
        QObject::connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueChangedInComboBox(int)));
      }    
    }

    if(m_comboBox == NULL)
    {
      m_lineEdit = new QLineEdit(this);
      hbox->addWidget(m_lineEdit);
      QObject::connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
    }
  }

  if(m_lineEdit)
    m_lineEdit->setText(s);
  if(m_comboBox)
  {
    for(int i=0;i<m_comboBox->count();i++)
    {
      if(m_comboBox->itemText(i) == s)
        m_comboBox->setCurrentIndex(i);
    }
  }
}

void StringValueWidget::setEnabled(bool state)
{
  if(m_lineEdit)
    m_lineEdit->setEnabled(state);
  if(m_comboBox)
    m_comboBox->setEnabled(state);
}

void StringValueWidget::onValueChangedInLineEdit()
{
  if(m_changingValue)
    return;
  m_changingValue = true;

  QString s = m_lineEdit->text();
  m_value = FabricCore::RTVal::ConstructString(*((ValueItem*)item())->client(), s.toUtf8().constData());
  emit dataChanged();

  m_changingValue = false;
}

void StringValueWidget::onValueChangedInComboBox(int index)
{
  if(m_changingValue)
    return;
  m_changingValue = true;

  QString s = m_comboBox->currentText();
  m_value = FabricCore::RTVal::ConstructString(*((ValueItem*)item())->client(), s.toUtf8().constData());
  emit dataChanged();

  m_changingValue = false;
}

TreeEditorWidget * StringValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  StringValueWidget * widget = new StringValueWidget(item->label().c_str(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool StringValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(item->type() != "ValueItem")
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "String";
}
