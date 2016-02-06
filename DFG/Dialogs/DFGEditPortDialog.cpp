// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

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

  m_visibilityCombo = new QComboBox(this);
  m_visibilityCombo->addItem("normal");
  m_visibilityCombo->addItem("opaque");
  m_visibilityCombo->addItem("hidden");
  m_persistValue = new QCheckBox(this);
  m_hasSoftRange = new QCheckBox(this);
  m_softRangeMin = new QLineEdit("0.0", this);
  m_softRangeMax = new QLineEdit("1.0", this);
  m_hasHardRange = new QCheckBox(this);
  m_hardRangeMin = new QLineEdit("0.0", this);
  m_hardRangeMax = new QLineEdit("1.0", this);
  m_hasCombo = new QCheckBox(this);
  m_combo = new QLineEdit("OptionA, OptionB", this);

  m_softRangeMin->setEnabled(false);
  m_softRangeMax->setEnabled(false);
  m_hardRangeMin->setEnabled(false);
  m_hardRangeMax->setEnabled(false);
  m_combo->setEnabled(false);
  m_softRangeMin->setValidator(new QDoubleValidator(m_softRangeMin));
  m_softRangeMax->setValidator(new QDoubleValidator(m_softRangeMax));
  m_hardRangeMin->setValidator(new QDoubleValidator(m_hardRangeMin));
  m_hardRangeMax->setValidator(new QDoubleValidator(m_hardRangeMax));

  if(m_portTypeCombo)
    addInput(m_portTypeCombo, "type", "required");
  addInput(m_titleEdit, "title", "required");
  if( topGraphPort ) {
    addInput( m_dataTypeEdit, "data type", "required" );
    addInput( m_extensionEdit, "extension", "advanced" );
  }
  addInput(m_visibilityCombo, "visibility", "metadata");
  addInput(m_persistValue, "persist value", "metadata");
  addInput(m_hasSoftRange, "soft range", "metadata");
  addInput(m_softRangeMin, "soft min", "metadata");
  addInput(m_softRangeMax, "soft max", "metadata");
  addInput(m_hasHardRange, "hard range", "metadata");
  addInput(m_hardRangeMin, "hard min", "metadata");
  addInput(m_hardRangeMax, "hard max", "metadata");
  addInput(m_hasCombo, "use combo", "metadata");
  addInput(m_combo, "combo", "metadata");

  // [Julien] FE-5188, FE-5276
  if(setAlphaNum) alphaNumicStringOnly();


  QObject::connect(m_hasSoftRange, SIGNAL(stateChanged(int)), this, SLOT(onSoftRangeToggled(int)));
  QObject::connect(m_hasHardRange, SIGNAL(stateChanged(int)), this, SLOT(onHardRangeToggled(int)));
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

bool DFGEditPortDialog::persistValue() const
{
  return m_persistValue->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setPersistValue(bool value)
{
  m_persistValue->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

bool DFGEditPortDialog::hasSoftRange() const
{
  return m_hasSoftRange->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasSoftRange(bool value)
{
  m_hasSoftRange->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

float DFGEditPortDialog::softRangeMin() const
{
  return m_softRangeMin->text().toFloat();
}

void DFGEditPortDialog::setSoftRangeMin(float value)
{
  m_softRangeMin->setText(QString::number(value));
}

float DFGEditPortDialog::softRangeMax() const
{
  return m_softRangeMax->text().toFloat();
}

void DFGEditPortDialog::setSoftRangeMax(float value)
{
  m_softRangeMax->setText(QString::number(value));
}

bool DFGEditPortDialog::hasHardRange() const
{
  return m_hasHardRange->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasHardRange(bool value)
{
  m_hasHardRange->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

float DFGEditPortDialog::hardRangeMin() const
{
  return m_hardRangeMin->text().toFloat();
}

void DFGEditPortDialog::setHardRangeMin(float value)
{
  m_hardRangeMin->setText(QString::number(value));
}

float DFGEditPortDialog::hardRangeMax() const
{
  return m_hardRangeMax->text().toFloat();
}

void DFGEditPortDialog::setHardRangeMax(float value)
{
  m_hardRangeMax->setText(QString::number(value));
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
  setRegexFilter(QString("[a-zA-Z][_a-zA-Z0-9]*"));
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

void DFGEditPortDialog::onSoftRangeToggled(int state)
{
  m_softRangeMin->setEnabled(state == Qt::Checked);
  m_softRangeMax->setEnabled(state == Qt::Checked);
}

void DFGEditPortDialog::onHardRangeToggled(int state)
{
  m_hardRangeMin->setEnabled(state == Qt::Checked);
  m_hardRangeMax->setEnabled(state == Qt::Checked);
}

void DFGEditPortDialog::onComboToggled(int state)
{
  m_combo->setEnabled(state == Qt::Checked);
}
