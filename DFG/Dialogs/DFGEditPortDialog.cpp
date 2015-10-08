// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include <QtGui/QDoubleValidator>
#include <QtCore/QTimer>

#include "DFGEditPortDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGEditPortDialog::DFGEditPortDialog(
  QWidget * parent, 
  FabricCore::Client & client, 
  bool showPortType, 
  bool topGraphPort, 
  const DFGConfig & dfgConfig,
  bool setAlphaNum)
: DFGBaseDialog(parent, true, dfgConfig)
{
  setWindowTitle("Edit Port");

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

  if( topGraphPort ) {
    m_dataTypeEdit = new DFGRegisteredTypeLineEdit( this, client, "" );
    m_extensionEdit = new DFGExtensionLineEdit( this, client );
  } else {
    m_dataTypeEdit = NULL;
    m_extensionEdit = NULL;
  }

  m_persistValue = new QCheckBox(this);
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

  if(m_portTypeCombo)
    addInput(m_portTypeCombo, "type", "required");
  addInput(m_titleEdit, "title", "required");
  if( topGraphPort ) {
    addInput( m_dataTypeEdit, "data type", "required" );
    addInput( m_extensionEdit, "extension", "advanced" );
  }
  addInput(m_persistValue, "persist value", "metadata");
  addInput(m_hasRange, "use range", "metadata");
  addInput(m_rangeMin, "range min", "metadata");
  addInput(m_rangeMax, "range max", "metadata");
  addInput(m_hasCombo, "use combo", "metadata");
  addInput(m_combo, "combo", "metadata");

  // [Julien] FE-5188, FE-5276
  if(setAlphaNum) alphaNumicStringOnly();


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
  return m_dataTypeEdit ? m_dataTypeEdit->text() : QString();
}

void DFGEditPortDialog::setDataType(QString value)
{
  if( m_dataTypeEdit )
    m_dataTypeEdit->setText( value );
  else
    assert( false );
}

QString DFGEditPortDialog::extension() const
{
  return m_extensionEdit ? m_extensionEdit->text() : QString();
}

void DFGEditPortDialog::setExtension(QString value)
{
  if( m_extensionEdit )
    m_extensionEdit->setText( value );
  else
    assert( false );
}

bool DFGEditPortDialog::persistValue() const
{
  return m_persistValue->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setPersistValue(bool value)
{
  m_persistValue->setCheckState(value ? Qt::Checked : Qt::Unchecked);
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
  m_rangeMin->setText(QString::number(value));
}

float DFGEditPortDialog::rangeMax() const
{
  return m_rangeMax->text().toFloat();
}

void DFGEditPortDialog::setRangeMax(float value)
{
  m_rangeMax->setText(QString::number(value));
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
  for(int i=0;i<results.length();i++)
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
  for(int i=0;i<value.length();i++)
  {
    if(i > 0)
      flat += ", ";
    flat += value[i];
  }
  m_combo->setText(flat);

}

void DFGEditPortDialog::showEvent(QShowEvent * event)
{
  QTimer::singleShot(0, m_titleEdit, SLOT(setFocus()));
  DFGBaseDialog::showEvent(event);  
}

// Allows only alpha-numeric text only 
void DFGEditPortDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("^[a-zA-Z0-9]*$*"));
}

// Filters the QLineEdit text with the regexFilter
void DFGEditPortDialog::setRegexFilter(QString regexFilter) {
  if(m_titleEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_titleEdit->setValidator(validator);
  }
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
