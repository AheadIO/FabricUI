// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ScalarValueWidget.h"
#include "ValueItem.h"

#include <QtGui/QDoubleValidator>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

ScalarValueWidget::ScalarValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();
  m_changingValue = false;

  m_lineEdit = new QLineEdit(this);
  QDoubleValidator * validator = new QDoubleValidator(m_lineEdit);
  validator->setDecimals(3);
  m_lineEdit->setValidator(validator);
  m_lineEdit->setMinimumWidth(60);
  m_lineEdit->setMaximumWidth(60);
  hbox->addWidget(m_lineEdit);
  m_slider = new QSlider(this);
  m_slider->setOrientation(Qt::Horizontal);
  m_slider->setMinimum(0);
  m_slider->setMaximum(10000);
  hbox->addWidget(m_slider);

  QObject::connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedInSlider()));
  QObject::connect(m_slider, SIGNAL(sliderPressed()), this, SLOT(onBeginInteraction()));
  QObject::connect(m_slider, SIGNAL(sliderReleased()), this, SLOT(onEndInteraction()));

  setRange(0.0f, 1.0f);
}

ScalarValueWidget::~ScalarValueWidget()
{
}

float ScalarValueWidget::getMinimum() const
{
  return m_minimum;
}

float ScalarValueWidget::getMaximum() const
{
  return m_maximum;
}

void ScalarValueWidget::setRange(float minimum, float maximum)
{
  m_minimum = minimum;
  m_maximum = maximum;
}

void ScalarValueWidget::setValue(FabricCore::RTVal v)
{
  if(m_changingValue)
    return;
  m_changingValue = true;

  ValueWidget::setValue(v);

  m_typeName = v.getTypeName().getStringCString();
  float f = 0.0;
  if(m_typeName == "Float32" || m_typeName == "Scalar")
    f = v.getFloat32();
  else if(m_typeName == "Float64")
    f = (float)v.getFloat64();

  if(f <= m_minimum)
    m_minimum = f;
  else if(f >= m_maximum)
    m_maximum = f;

  float ratio = (f - m_minimum)  / (m_maximum - m_minimum);
  m_slider->setValue(10000.0f * ratio);

  m_lineEdit->setText(QString::number(f));

  m_changingValue = false;
}

void ScalarValueWidget::setEnabled(bool state)
{
  m_lineEdit->setEnabled(state);
  m_slider->setEnabled(state);
}

void ScalarValueWidget::onValueChangedInLineEdit()
{
  if(m_changingValue)
    return;
  m_changingValue = true;
  QString text = m_lineEdit->text();
  float f = text.toFloat();
  if(f <= m_minimum)
    m_minimum = f;
  else if(f >= m_maximum)
    m_maximum = f;

  float ratio = (f - m_minimum)  / (m_maximum - m_minimum);
  m_slider->setValue(10000.0f * ratio);

  if(m_typeName == "Float32" || m_typeName == "Scalar")
    m_value = FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), f);
  else
    m_value = FabricCore::RTVal::ConstructFloat64(*((ValueItem*)item())->client(), f);

  emit dataChanged();
  m_changingValue = false;
}

void ScalarValueWidget::onValueChangedInSlider()
{
  if(m_changingValue)
    return;
  m_changingValue = true;
  float ratio = float(m_slider->value()) / 10000.0f;
  float f = m_minimum + (m_maximum - m_minimum) * ratio;
  m_lineEdit->setText(QString::number(f));

  if(m_typeName == "Float32" || m_typeName == "Scalar")
    m_value = FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), f);
  else
    m_value = FabricCore::RTVal::ConstructFloat64(*((ValueItem*)item())->client(), f);

  emit dataChanged();
  m_changingValue = false;
}

void ScalarValueWidget::onBeginInteraction()
{
  emit ValueWidget::beginInteraction(valueItem());
}

void ScalarValueWidget::onEndInteraction()
{
  emit ValueWidget::endInteraction(valueItem());
}

TreeEditorWidget * ScalarValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  ScalarValueWidget * widget = new ScalarValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool ScalarValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(!ValueWidget::canDisplay(item))
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "Float32" || typeName == "Float64" || typeName == "Scalar";
}
