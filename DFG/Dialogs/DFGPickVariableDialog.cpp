// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGPickVariableDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

/// Constructor
DFGPickVariableDialog::DFGPickVariableDialog(
  QWidget * parent,
  FabricCore::Client & client,
  FabricCore::DFGBinding & binding,
  FTL::CStrRef currentExecPath,
  bool setAlphaNum,
  QString name,
  const DFGConfig & dfgConfig
  )
: DFGBaseDialog(parent, true, dfgConfig)
{
  setWindowTitle("Pick Variable");
  m_nameCombo = new DFGVariablePathComboBox(this, binding, currentExecPath, name);
  addInput(m_nameCombo, "name");
  // [Julien] Allows only alpha-numeric text only 
  // We do this because the nodes's name must be alpha-numerical only
  // and not contains "-, +, ?,"
  if(setAlphaNum) alphaNumicStringOnly();
}

/// Destuctorle
DFGPickVariableDialog::~DFGPickVariableDialog()
{
}

// Allows only alpha-numeric text only 
void DFGPickVariableDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("([a-zA-Z][_a-zA-Z0-9]*\\.)*[a-zA-Z][_a-zA-Z0-9]*"));
}

// Filters the QLineEdit text with the regexFilter
void DFGPickVariableDialog::setRegexFilter(QString regexFilter) {
  if(m_nameCombo)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_nameCombo->setValidator(validator);
  }
}

/// Gets the name of the variable
QString DFGPickVariableDialog::name() const
{
  return m_nameCombo->currentText();
}
