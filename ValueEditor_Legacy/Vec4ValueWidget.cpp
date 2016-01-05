// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Vec4ValueWidget.h"
#include "ValueItem.h"
#include "ValueEditorEventFilters.h"

#include <QtGui/QDoubleValidator>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor_Legacy;

Vec4ValueWidget::Vec4ValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_lineEditX = new LineEdit(this);
  m_lineEditY = new LineEdit(this);
  m_lineEditZ = new LineEdit(this);
  m_lineEditT = new LineEdit(this);
  QDoubleValidator * validator = new QDoubleValidator(this);
  validator->setDecimals(3);
  m_lineEditX->setValidator(validator);
  m_lineEditY->setValidator(validator);
  m_lineEditZ->setValidator(validator);
  m_lineEditT->setValidator(validator);
  hbox->addWidget(m_lineEditX);
  hbox->addWidget(m_lineEditY);
  hbox->addWidget(m_lineEditZ);
  hbox->addWidget(m_lineEditT);

  m_lineEditX->setFocusPolicy(Qt::StrongFocus);
  m_lineEditY->setFocusPolicy(Qt::StrongFocus);
  m_lineEditZ->setFocusPolicy(Qt::StrongFocus);
  m_lineEditT->setFocusPolicy(Qt::StrongFocus);

  QObject::connect(m_lineEditX, SIGNAL(lineEditTextEdited(const QString&)), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditY, SIGNAL(lineEditTextEdited(const QString&)), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditZ, SIGNAL(lineEditTextEdited(const QString&)), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditT, SIGNAL(lineEditTextEdited(const QString&)), this, SLOT(onValueChangedInLineEdit()));
}

Vec4ValueWidget::~Vec4ValueWidget()
{
}

void Vec4ValueWidget::onValueChangedInLineEdit()
{
  float x = m_lineEditX->text().toFloat();
  float y = m_lineEditY->text().toFloat();
  float z = m_lineEditZ->text().toFloat();
  float t = m_lineEditT->text().toFloat();
  m_value = FabricCore::RTVal::Construct(*((ValueItem*)item())->client(), "Vec4", 0, 0);
  m_value.setMember("x", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), x));
  m_value.setMember("y", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), y));
  m_value.setMember("z", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), z));
  m_value.setMember("t", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), t));
  ValueWidget::setValue(m_value);
}

void Vec4ValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);

  float x = value().maybeGetMember("x").getFloat32();
  float y = value().maybeGetMember("y").getFloat32();
  float z = value().maybeGetMember("z").getFloat32();
  float t = value().maybeGetMember("t").getFloat32();
  m_lineEditX->setLineEditText(QString::number(x));
  m_lineEditY->setLineEditText(QString::number(y));
  m_lineEditZ->setLineEditText(QString::number(z));
  m_lineEditT->setLineEditText(QString::number(t));
}

void Vec4ValueWidget::setEnabled(bool state)
{
  m_lineEditX->setEnabled(state);
  m_lineEditY->setEnabled(state);
  m_lineEditZ->setEnabled(state);
  m_lineEditT->setEnabled(state);
}

void Vec4ValueWidget::onBeginInteraction()
{
  emit ValueWidget::beginInteraction(valueItem());
}

void Vec4ValueWidget::onEndInteraction()
{
  emit ValueWidget::endInteraction(valueItem());
}

TreeEditorWidget * Vec4ValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  Vec4ValueWidget * widget = new Vec4ValueWidget(item->label().c_str(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool Vec4ValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(item->type() != "ValueItem")
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "Vec4";
}
