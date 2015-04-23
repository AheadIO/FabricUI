// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include <QtGui/QDoubleValidator>
#include "DFGEditPortDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGEditPortDialog::DFGEditPortDialog(QWidget * parent, bool showPortType, const DFGConfig & dfgConfig)
: DFGBaseDialog(parent, true, dfgConfig)
{
  if(showPortType)
  {
    m_portTypeCombo = new QComboBox(this);
    m_portTypeCombo->addItem("In");
    m_portTypeCombo->addItem("Out");
    m_portTypeCombo->addItem("IO");
  }
  else
    m_portTypeCombo = NULL;
  m_titleEdit = new QLineEdit("", this);
  m_dataTypeEdit = new QLineEdit("$TYPE$", this);
  m_extensionEdit = new QLineEdit("", this);

  m_native = new QCheckBox(this);
  m_visibilityCombo = new QComboBox(this);
  m_visibilityCombo->addItem("normal");
  m_visibilityCombo->addItem("opaque");
  m_visibilityCombo->addItem("hidden");
  m_hasRange = new QCheckBox(this);
  m_rangeMin = new QLineEdit("0.0", this);
  m_rangeMax = new QLineEdit("1.0", this);
  m_hasCombo = new QCheckBox(this);
  m_combo = new QLineEdit("OptionA, OptionB", this);

  m_rangeMin->setEnabled(false);
  m_rangeMax->setEnabled(false);
  m_combo->setEnabled(false);
  m_rangeMin->setValidator(new QDoubleValidator(m_rangeMin));
  m_rangeMax->setValidator(new QDoubleValidator(m_rangeMax));

  if(showPortType)
    addInput(m_portTypeCombo, "type");
  addInput(m_titleEdit, "title");
  addInput(m_dataTypeEdit, "data type");
  addInput(m_extensionEdit, "extension");
  addInput(m_native, "native DCC array");
  addInput(m_visibilityCombo, "visibility");
  addInput(m_hasRange, "use range");
  addInput(m_rangeMin, "range min");
  addInput(m_rangeMax, "range max");
  addInput(m_hasCombo, "use combo");
  addInput(m_combo, "combo");

  QObject::connect(m_hasRange, SIGNAL(stateChanged(int)), this, SLOT(onRangeToggled(int)));
  QObject::connect(m_hasCombo, SIGNAL(stateChanged(int)), this, SLOT(onComboToggled(int)));
}

DFGEditPortDialog::~DFGEditPortDialog()
{
}

QString DFGEditPortDialog::portType() const
{
  if(!m_portTypeCombo)
    return "";
  return m_portTypeCombo->currentText();
}

void DFGEditPortDialog::setPortType(QString value)
{
  if(!m_portTypeCombo)
    return;
  if(value.toLower() == "in")
    m_portTypeCombo->setCurrentIndex(0);
  else if(value.toLower() == "out")
    m_portTypeCombo->setCurrentIndex(1);
  else if(value.toLower() == "io")
    m_portTypeCombo->setCurrentIndex(2);
}

QString DFGEditPortDialog::title() const
{
  return m_titleEdit->text();
}

void DFGEditPortDialog::setTitle(QString value)
{
  m_titleEdit->setText(value);
}

QString DFGEditPortDialog::dataType() const
{
  return m_dataTypeEdit->text();
}

void DFGEditPortDialog::setDataType(QString value)
{
  m_dataTypeEdit->setText(value);
}

QString DFGEditPortDialog::extension() const
{
  return m_extensionEdit->text();
}

void DFGEditPortDialog::setExtension(QString value)
{
  m_extensionEdit->setText(value);
}

bool DFGEditPortDialog::native() const
{
  return m_native->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setNative(bool value)
{
  m_native->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

bool DFGEditPortDialog::hidden() const
{
  return m_visibilityCombo->currentText() == "hidden";
}

void DFGEditPortDialog::setHidden()
{
  m_visibilityCombo->setCurrentIndex(2);
}

bool DFGEditPortDialog::opaque() const
{
  return m_visibilityCombo->currentText() == "opaque";
}

void DFGEditPortDialog::setOpaque()
{
  m_visibilityCombo->setCurrentIndex(1);
}

bool DFGEditPortDialog::hasRange() const
{
  return m_hasRange->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasRange(bool value)
{
  m_hasRange->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

float DFGEditPortDialog::rangeMin() const
{
  return m_rangeMin->text().toFloat();
}

void DFGEditPortDialog::setRangeMin(float value)
{

}

float DFGEditPortDialog::rangeMax() const
{
  return m_rangeMax->text().toFloat();
}

void DFGEditPortDialog::setRangeMax(float value)
{

}

bool DFGEditPortDialog::hasCombo() const
{
  return m_hasCombo->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasCombo(bool value)
{
  m_hasCombo->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

QStringList DFGEditPortDialog::comboValues() const
{
  QStringList results = m_combo->text().split(',');
  for(unsigned int i=0;i<results.length();i++)
  {
    results[i] = results[i].trimmed();
    if(results[i][0] == '\'' || results[i][0] == '"')
      results[i] = results[i].right(results[i].length()-1);
    if(results[i][results[i].length()-1] == '\'' || results[i][results[i].length()-1] == '"')
      results[i] = results[i].left(results[i].length()-1);
  }
  return results;
}

void DFGEditPortDialog::setComboValues(QStringList value)
{
  QString flat;
  for(unsigned int i=0;i<value.length();i++)
  {
    if(i > 0)
      flat += ", ";
    flat += value[i];
  }
  m_combo->setText(flat);

}

void DFGEditPortDialog::onRangeToggled(int state)
{
  m_rangeMin->setEnabled(state == Qt::Checked);
  m_rangeMax->setEnabled(state == Qt::Checked);
}

void DFGEditPortDialog::onComboToggled(int state)
{
  m_combo->setEnabled(state == Qt::Checked);
}
