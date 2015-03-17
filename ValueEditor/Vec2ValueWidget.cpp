// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Vec2ValueWidget.h"
#include "ValueItem.h"

#include <QtGui/QDoubleValidator>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

Vec2ValueWidget::Vec2ValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_lineEditX = new QLineEdit(this);
  m_lineEditY = new QLineEdit(this);
  QDoubleValidator * validator = new QDoubleValidator(this);
  validator->setDecimals(3);
  m_lineEditX->setValidator(validator);
  m_lineEditY->setValidator(validator);
  hbox->addWidget(m_lineEditX);
  hbox->addWidget(m_lineEditY);

  QObject::connect(m_lineEditX, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditY, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
}

Vec2ValueWidget::~Vec2ValueWidget()
{
}

void Vec2ValueWidget::onValueChangedInLineEdit()
{
  float x = m_lineEditX->text().toFloat();
  float y = m_lineEditY->text().toFloat();
  m_value = FabricCore::RTVal::Construct(*((ValueItem*)item())->client(), "Vec2", 0, 0);
  m_value.setMember("x", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), x));
  m_value.setMember("y", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), y));
  emit dataChanged();
}

void Vec2ValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);

  float x = value().maybeGetMember("x").getFloat32();
  float y = value().maybeGetMember("y").getFloat32();
  m_lineEditX->setText(QString::number(x));
  m_lineEditY->setText(QString::number(y));
}

void Vec2ValueWidget::setEnabled(bool state)
{
  m_lineEditX->setEnabled(state);
  m_lineEditY->setEnabled(state);
}

void Vec2ValueWidget::onBeginInteraction()
{
  emit ValueWidget::beginInteraction(valueItem());
}

void Vec2ValueWidget::onEndInteraction()
{
  emit ValueWidget::endInteraction(valueItem());
}

TreeEditorWidget * Vec2ValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  Vec2ValueWidget * widget = new Vec2ValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool Vec2ValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(!ValueWidget::canDisplay(item))
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "Vec2";
}
