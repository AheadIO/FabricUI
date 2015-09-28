// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ColorValueWidget.h"
#include "ValueItem.h"
#include "ValueEditorEventFilters.h"

#include <QtGui/QDoubleValidator>

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

  m_dialog = new QColorDialog( this );
  m_dialog->setOptions(QColorDialog::ShowAlphaChannel);
  m_dialog->setModal(true);
}

void ColorPickerWidget::mousePressEvent ( QMouseEvent * event )
{
  if(m_enabled)
  {
    m_prevColorR = m_colorR;
    m_prevColorG = m_colorG;
    m_prevColorB = m_colorB;
    m_prevColorA = m_colorA;

    m_dialog->setCurrentColor(
      QColor( int( m_colorR * 255.0f ), int( m_colorG * 255.0f ),
              int( m_colorB * 255.0f ), int( m_colorA * 255.0f ) ) );
    connect(m_dialog, SIGNAL(rejected()), this, SLOT(colorDialogRejected()));
    connect(m_dialog, SIGNAL(currentColorChanged(const QColor &)), this, SLOT(colorDialogChanged(const QColor &)));

    m_dialog->show();
    return;
  }

  QWidget::mousePressEvent(event);
}

void ColorPickerWidget::paintEvent ( QPaintEvent * event )
{
  QPainter painter;
  painter.begin(this);
  painter.fillRect(
    event->rect(),
    QBrush( QColor( int( m_colorR * 255.0f ), int( m_colorG * 255.0f ),
                    int( m_colorB * 255.0f ) ) ) );
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
  m_colorR = m_prevColorR;
  m_colorG = m_prevColorG;
  m_colorB = m_prevColorB;
  m_colorA = m_prevColorA;
  emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
  update();
}

ColorValueWidget::ColorValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_colorPicker = new ColorPickerWidget(this);
  m_lineEditR = new LineEdit(this);
  m_lineEditG = new LineEdit(this);
  m_lineEditB = new LineEdit(this);
  m_lineEditA = new LineEdit(this);
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
  m_colorPicker->set(0, 0, 0, 0, true);

  m_lineEditR->setFocusPolicy(Qt::StrongFocus);
  m_lineEditG->setFocusPolicy(Qt::StrongFocus);
  m_lineEditB->setFocusPolicy(Qt::StrongFocus);
  m_lineEditA->setFocusPolicy(Qt::StrongFocus);
  m_lineEditA->setFocusPolicy(Qt::StrongFocus);
  m_colorPicker->setFocusPolicy(Qt::NoFocus);

  m_colorPicker->installEventFilter(new BackspaceDeleteEventFilter(this));

  hbox->addWidget(m_lineEditR);
  hbox->addWidget(m_lineEditG);
  hbox->addWidget(m_lineEditB);
  hbox->addWidget(m_lineEditA);
  hbox->addWidget(m_colorPicker);

  QObject::connect(m_lineEditR, SIGNAL(lineEditTextEdited(const QString &)), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditG, SIGNAL(lineEditTextEdited(const QString &)), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditB, SIGNAL(lineEditTextEdited(const QString &)), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_lineEditA, SIGNAL(lineEditTextEdited(const QString &)), this, SLOT(onValueChangedInLineEdit()));
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
  m_colorPicker->set(r, g, b, a, true);
  ValueWidget::setValue(genRtVal(r, g, b, a));
}

void ColorValueWidget::onValueChangedColorPicker(float r, float g, float b, float a)
{
  m_lineEditR->setLineEditText(QString::number(r));
  m_lineEditG->setLineEditText(QString::number(g));
  m_lineEditB->setLineEditText(QString::number(b));
  m_lineEditA->setLineEditText(QString::number(a));
  ValueWidget::setValue(genRtVal(r, g, b, a));
}

void ColorValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);
  m_typeName = v.getTypeName().getStringCString();

  float r, g, b, a;
  if(m_typeName == "Color")
  {
    r = value().maybeGetMember("r").getFloat32();
    g = value().maybeGetMember("g").getFloat32();
    b = value().maybeGetMember("b").getFloat32();
    a = value().maybeGetMember("a").getFloat32();
  }
  else if(m_typeName == "RGB")
  {
    r = float(value().maybeGetMember("r").getUInt8()) / 255.0f;
    g = float(value().maybeGetMember("g").getUInt8()) / 255.0f;
    b = float(value().maybeGetMember("b").getUInt8()) / 255.0f;
    a = 1.0;
  }
  else if(m_typeName == "RGBA" || m_typeName == "ARGB")
  {
    r = float(value().maybeGetMember("r").getUInt8()) / 255.0f;
    g = float(value().maybeGetMember("g").getUInt8()) / 255.0f;
    b = float(value().maybeGetMember("b").getUInt8()) / 255.0f;
    a = float(value().maybeGetMember("a").getUInt8()) / 255.0f;
  }
  else
  {
    r = g = b = 0.0;
    a = 1.0;
  }
  m_lineEditR->setLineEditText(QString::number(r));
  m_lineEditG->setLineEditText(QString::number(g));
  m_lineEditB->setLineEditText(QString::number(b));
  m_lineEditA->setLineEditText(QString::number(a));
  m_colorPicker->set(r, g, b, a, true);
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
  ColorValueWidget * widget = new ColorValueWidget(item->label().c_str(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool ColorValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(item->type() != "ValueItem")
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "Color" || 
    typeName == "RGB" ||
    typeName == "RGBA" || 
    typeName == "ARGB";
}

FabricCore::RTVal ColorValueWidget::genRtVal(float r, float g, float b, float a)
{
  FabricCore::RTVal val = FabricCore::RTVal::Construct(*((ValueItem*)item())->client(), m_typeName.c_str(), 0, 0);
  if(m_typeName == "Color")
  {
    val.setMember("r", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), r));
    val.setMember("g", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), g));
    val.setMember("b", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), b));
    val.setMember("a", FabricCore::RTVal::ConstructFloat32(*((ValueItem*)item())->client(), a));
  }
  else if(m_typeName == "RGB")
  {
    val.setMember("r", FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), int(r * 255.0f)));
    val.setMember("g", FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), int(g * 255.0f)));
    val.setMember("b", FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), int(b * 255.0f)));
  }
  else if(m_typeName == "RGBA" || m_typeName == "ARGB")
  {
    val.setMember("r", FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), int(r * 255.0f)));
    val.setMember("g", FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), int(g * 255.0f)));
    val.setMember("b", FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), int(b * 255.0f)));
    val.setMember("a", FabricCore::RTVal::ConstructUInt8(*((ValueItem*)item())->client(), int(a * 255.0f)));
  }
  return val;
}
