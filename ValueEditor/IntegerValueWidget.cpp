// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "IntegerValueWidget.h"
#include "ValueItem.h"

#include <QtGui/QIntValidator>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

IntegerValueWidget::IntegerValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();
  m_changingValue = false;

  m_lineEdit = new QLineEdit(this);
  QIntValidator * validator = new QIntValidator(m_lineEdit);
  m_lineEdit->setValidator(validator);
  m_lineEdit->setMinimumWidth(60);
  m_lineEdit->setMaximumWidth(60);
  hbox->addWidget(m_lineEdit);
  m_slider = new QSlider(this);
  m_slider->setOrientation(Qt::Horizontal);
  hbox->addWidget(m_slider);

  QObject::connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedInSlider()));
  QObject::connect(m_slider, SIGNAL(sliderPressed()), this, SLOT(onBeginInteraction()));
  QObject::connect(m_slider, SIGNAL(sliderReleased()), this, SLOT(onEndInteraction()));

  setRange(0, 100);
}

IntegerValueWidget::~IntegerValueWidget()
{
}

int IntegerValueWidget::getMinimum() const
{
  return m_slider->minimum();;
}

int IntegerValueWidget::getMaximum() const
{
  return m_slider->maximum();;
}

void IntegerValueWidget::setRange(int minimum, int maximum)
{
  m_slider->setMinimum(minimum);
  m_slider->setMaximum(maximum);
}

void IntegerValueWidget::setValue(FabricCore::RTVal v)
{
  if(m_changingValue)
    return;
  m_changingValue = true;

  ValueWidget::setValue(v);

  m_typeName = v.getTypeName().getStringCString();

  int i = 0;
  if(m_typeName == "SInt8")
    i = (int)v.getSInt8();
  else if(m_typeName == "SInt16")
    i = (int)v.getSInt16();
  else if(m_typeName == "SInt32" || m_typeName == "Integer")
    i = (int)v.getSInt32();
  else if(m_typeName == "SInt64")
    i = (int)v.getSInt64();
  else if(m_typeName == "UInt8" || m_typeName == "Byte")
    i = (int)v.getUInt8();
  else if(m_typeName == "UInt16")
    i = (int)v.getUInt16();
  else if(m_typeName == "UInt32" || m_typeName == "Index" || m_typeName == "Count" || m_typeName == "Size")
    i = (int)v.getUInt32();
  else if(m_typeName == "UInt64")
    i = (int)v.getUInt64();

  if(i < m_slider->minimum())
    m_slider->setMinimum(i);
  if(i > m_slider->maximum())
    m_slider->setMaximum(i);
  m_slider->setValue(i);
  m_lineEdit->setText(QString::number(i));

  m_changingValue = false;
}

void IntegerValueWidget::setEnabled(bool state)
{
  m_lineEdit->setEnabled(state);
  m_slider->setEnabled(state);
}

void IntegerValueWidget::onValueChangedInLineEdit()
{
  if(m_changingValue)
    return;
  m_changingValue = true;

  QString text = m_lineEdit->text();
  int i = text.toInt();
  if(i < m_slider->minimum())
    m_slider->setMinimum(i);
  if(i > m_slider->maximum())
    m_slider->setMaximum(i);
  m_slider->setValue(i);

  if(m_typeName == "SInt8")
    m_value = FabricCore::RTVal::ConstructSInt8(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "SInt16")
    m_value = FabricCore::RTVal::ConstructSInt16(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "SInt32" || m_typeName == "Integer")
    m_value = FabricCore::RTVal::ConstructSInt32(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "SInt64")
    m_value = FabricCore::RTVal::ConstructSInt64(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt8" || m_typeName == "Byte")
    m_value = FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt16")
    m_value = FabricCore::RTVal::ConstructUInt16(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt32" || m_typeName == "Index" || m_typeName == "Count" || m_typeName == "Size")
    m_value = FabricCore::RTVal::ConstructUInt32(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt64")
    m_value = FabricCore::RTVal::ConstructUInt64(*((ValueItem*)item())->client(), i);
  
  emit dataChanged();

  m_changingValue = false;
}

void IntegerValueWidget::onValueChangedInSlider()
{
  if(m_changingValue)
    return;
  m_changingValue = true;

  int i = m_slider->value();
  m_lineEdit->setText(QString::number(i));

  if(m_typeName == "SInt8")
    m_value = FabricCore::RTVal::ConstructSInt8(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "SInt16")
    m_value = FabricCore::RTVal::ConstructSInt16(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "SInt32" || m_typeName == "Integer")
    m_value = FabricCore::RTVal::ConstructSInt32(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "SInt64")
    m_value = FabricCore::RTVal::ConstructSInt64(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt8" || m_typeName == "Byte")
    m_value = FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt16")
    m_value = FabricCore::RTVal::ConstructUInt16(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt32" || m_typeName == "Index" || m_typeName == "Count" || m_typeName == "Size")
    m_value = FabricCore::RTVal::ConstructUInt32(*((ValueItem*)item())->client(), i);
  else if(m_typeName == "UInt64")
    m_value = FabricCore::RTVal::ConstructUInt64(*((ValueItem*)item())->client(), i);

  emit dataChanged();
  m_changingValue = false;
}

void IntegerValueWidget::onBeginInteraction()
{
  emit ValueWidget::beginInteraction(valueItem());
}

void IntegerValueWidget::onEndInteraction()
{
  emit ValueWidget::endInteraction(valueItem());
}

TreeEditorWidget * IntegerValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  IntegerValueWidget * widget = new IntegerValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool IntegerValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(!ValueWidget::canDisplay(item))
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();

  // todo: this should work for any int
  return 
    typeName == "SInt8" || 
    typeName == "SInt16" || 
    typeName == "SInt32" || 
    typeName == "SInt64" || 
    typeName == "UInt8" || 
    typeName == "UInt16" || 
    typeName == "UInt32" || 
    typeName == "UInt64" || 
    typeName == "Byte" || 
    typeName == "Size" || 
    typeName == "Count" || 
    typeName == "Index" || 
    typeName == "Integer";
}
