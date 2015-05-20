// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ColorValueWidget.h"
#include "ValueItem.h"

#include <QtGui/QDoubleValidator>
#include <QtGui/QColorDialog>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

ColorPickerWidget::ColorPickerWidget(QWidget * parent)
  : QWidget(parent)
{
  setMinimumSize(60, 25);
  setMaximumSize(100, 25);
  m_enabled = true;
  m_colorR = 1.0;
  m_colorG = 0.0;
  m_colorB = 0.0;
  m_colorA = 1.0;
}

void ColorPickerWidget::mousePressEvent ( QMouseEvent * event )
{
  if(m_enabled)
  {
    float prevR = m_colorR;
    float prevG = m_colorG;
    float prevB = m_colorB;
    float prevA = m_colorA;
    m_dialogRejected = false;

    QColorDialog dialog( this );
    dialog.setOptions(QColorDialog::ShowAlphaChannel);
    dialog.setModal(true);
    dialog.setCurrentColor(QColor(m_colorR * 255.0f, m_colorG * 255.0f, m_colorB * 255.0f, m_colorA * 255.0f));
    connect(&dialog, SIGNAL(rejected()), this, SLOT(colorDialogRejected()));
    connect(&dialog, SIGNAL(currentColorChanged(const QColor &)), this, SLOT(colorDialogChanged(const QColor &)));

    dialog.exec();

    if(m_dialogRejected)
    {
      m_colorR = prevR;
      m_colorG = prevG;
      m_colorB = prevB;
      m_colorA = prevA;
      emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
      update();
    }
    return;
  }

  QWidget::mousePressEvent(event);
}

void ColorPickerWidget::paintEvent ( QPaintEvent * event )
{
  QPainter painter;
  painter.begin(this);
  painter.fillRect(event->rect(), QBrush(QColor(m_colorR * 255.0f, m_colorG * 255.0f, m_colorB * 255.0f)));
  painter.end();  
}

void ColorPickerWidget::colorDialogChanged(const QColor & color)
{
  m_colorR = color.redF();
  m_colorG = color.greenF();
  m_colorB = color.blueF();
  m_colorA = color.alphaF();
  emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
  update();
}

void ColorPickerWidget::colorDialogRejected()
{
  m_dialogRejected = true;
}

ColorValueWidget::ColorValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_colorPicker = new ColorPickerWidget(this);
  m_lineEditR = new QLineEdit(this);
  m_lineEditG = new QLineEdit(this);
  m_lineEditB = new QLineEdit(this);
  m_lineEditA = new QLineEdit(this);
  QDoubleValidator * validator = new QDoubleValidator(this);
  validator->setDecimals(3);
  m_lineEditR->setValidator(validator);
  m_lineEditG->setValidator(validator);
  m_lineEditB->setValidator(validator);
  m_lineEditA->setValidator(validator);

  m_colorPicker->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
  m_colorPicker->setContentsMargins(0, 0, 0, 0);
  m_colorPicker->setMinimumWidth(60);
  m_colorPicker->setMinimumHeight(m_lineEditR->minimumHeight());
  m_colorPicker->set(0, 0, 0, 0);

  hbox->addWidget(m_lineEditR);
  hbox->addWidget(m_lineEditG);
  hbox->addWidget(m_lineEditB);
  hbox->addWidget(m_lineEditA);
  hbox->addWidget(m_colorPicker);

  QObject::connect(m_lineEditR, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditG, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditB, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditA, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_colorPicker, SIGNAL( colorChanged(float, float, float, float) ), this , SLOT( onValueChangedColorPicker(float, float, float, float) ) );
}

ColorValueWidget::~ColorValueWidget()
{
}

void ColorValueWidget::onValueChangedInLineEdit()
{
  float r = m_lineEditR->text().toFloat();
  float g = m_lineEditG->text().toFloat();
  float b = m_lineEditB->text().toFloat();
  float a = m_lineEditA->text().toFloat();
  m_value = FabricCore::RTVal::Construct(*((ValueItem*)item())->client(), "Color", 0, 0);
  m_value.setMember("r", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), r));
  m_value.setMember("g", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), g));
  m_value.setMember("b", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), b));
  m_value.setMember("a", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), a));
  m_colorPicker->set(r, g, b, a);
  emit dataChanged();
}

void ColorValueWidget::onValueChangedColorPicker(float r, float g, float b, float a)
{
  m_value = FabricCore::RTVal::Construct(*((ValueItem*)item())->client(), "Color", 0, 0);
  m_value.setMember("r", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), r));
  m_value.setMember("g", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), g));
  m_value.setMember("b", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), b));
  m_value.setMember("a", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), a));
  m_lineEditR->setText(QString::number(r));
  m_lineEditG->setText(QString::number(g));
  m_lineEditB->setText(QString::number(b));
  m_lineEditA->setText(QString::number(a));
  emit dataChanged();
}

void ColorValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);

  float r = value().maybeGetMember("r").getFloat32();
  float g = value().maybeGetMember("g").getFloat32();
  float b = value().maybeGetMember("b").getFloat32();
  float a = value().maybeGetMember("a").getFloat32();
  m_lineEditR->setText(QString::number(r));
  m_lineEditG->setText(QString::number(g));
  m_lineEditB->setText(QString::number(b));
  m_lineEditA->setText(QString::number(a));
  m_colorPicker->set(r, g, b, a);
}

void ColorValueWidget::setEnabled(bool state)
{
  m_lineEditR->setEnabled(state);
  m_lineEditG->setEnabled(state);
  m_lineEditB->setEnabled(state);
  m_lineEditA->setEnabled(state);
  m_colorPicker->setEnabled(state);
}

void ColorValueWidget::onBeginInteraction()
{
  emit ValueWidget::beginInteraction(valueItem());
}

void ColorValueWidget::onEndInteraction()
{
  emit ValueWidget::endInteraction(valueItem());
}

TreeEditorWidget * ColorValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  ColorValueWidget * widget = new ColorValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool ColorValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(!ValueWidget::canDisplay(item))
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "Color";
}
