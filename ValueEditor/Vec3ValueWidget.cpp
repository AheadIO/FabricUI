// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Vec3ValueWidget.h"
#include "ValueItem.h"

#include <QtGui/QDoubleValidator>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

Vec3ValueWidget::Vec3ValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_lineEditX = new QLineEdit(this);
  m_lineEditY = new QLineEdit(this);
  m_lineEditZ = new QLineEdit(this);
  QDoubleValidator * validator = new QDoubleValidator(this);
  validator->setDecimals(3);
  m_lineEditX->setValidator(validator);
  m_lineEditY->setValidator(validator);
  m_lineEditZ->setValidator(validator);
  hbox->addWidget(m_lineEditX);
  hbox->addWidget(m_lineEditY);
  hbox->addWidget(m_lineEditZ);

  QObject::connect(m_lineEditX, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditY, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditZ, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
}

Vec3ValueWidget::~Vec3ValueWidget()
{
}

void Vec3ValueWidget::onValueChangedInLineEdit()
{
  float x = m_lineEditX->text().toFloat();
  float y = m_lineEditY->text().toFloat();
  float z = m_lineEditZ->text().toFloat();
  m_value = FabricCore::RTVal::Construct(*((ValueItem*)item())->client(), "Vec3", 0, 0);
  m_value.setMember("x", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), x));
  m_value.setMember("y", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), y));
  m_value.setMember("z", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), z));
  emit dataChanged();
}

void Vec3ValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);

  float x = value().maybeGetMember("x").getFloat32();
  float y = value().maybeGetMember("y").getFloat32();
  float z = value().maybeGetMember("z").getFloat32();
  m_lineEditX->setText(QString::number(x));
  m_lineEditY->setText(QString::number(y));
  m_lineEditZ->setText(QString::number(z));
}

void Vec3ValueWidget::setEnabled(bool state)
{
  m_lineEditX->setEnabled(state);
  m_lineEditY->setEnabled(state);
  m_lineEditZ->setEnabled(state);
}

void Vec3ValueWidget::onBeginInteraction()
{
  emit ValueWidget::beginInteraction(valueItem());
}

void Vec3ValueWidget::onEndInteraction()
{
  emit ValueWidget::endInteraction(valueItem());
}

TreeEditorWidget * Vec3ValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  Vec3ValueWidget * widget = new Vec3ValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool Vec3ValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(!ValueWidget::canDisplay(item))
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "Vec3";
}
