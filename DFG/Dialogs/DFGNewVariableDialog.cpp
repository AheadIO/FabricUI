// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGNewVariableDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

/// Constructor
DFGNewVariableDialog::DFGNewVariableDialog(
  QWidget * parent,
  FabricCore::Client & client,
  FabricCore::DFGBinding & binding,
  FTL::CStrRef currentExecPath,
  bool setAlphaNum, 
  QString name,
  QString dataType,
  QString extension,
  const DFGConfig & dfgConfig
  )
: DFGBaseDialog(parent, true, dfgConfig)
{
  setWindowTitle("New Variable");

  m_nameEdit = new DFGVariablePathLineEdit(this, binding, currentExecPath, name);
  addInput(m_nameEdit, "name");
  m_dataTypeEdit = new DFGRegisteredTypeLineEdit(this, client, dataType);
  addInput(m_dataTypeEdit, "type");
  m_extensionEdit = new DFGExtensionLineEdit(this, client, dataType);
  addInput(m_extensionEdit, "extension");
  m_nameEdit->selectAll();

  // [Julien] Allows only alpha-numeric text only 
  // We do this because the nodes's name must be alpha-numerical only
  // and not contains "-, +, ?,"
  if(setAlphaNum) alphaNumicStringOnly();
}

/// Destuctor
DFGNewVariableDialog::~DFGNewVariableDialog()
{
}

// Allows only alpha-numeric text only 
void DFGNewVariableDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("^[a-zA-Z0-9]*$*"));
}

// Filters the QLineEdit text with the regexFilter
void DFGNewVariableDialog::setRegexFilter(QString regexFilter) {
  if(m_nameEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_nameEdit->setValidator(validator);
  }
}

/// Gets the name of the variable
QString DFGNewVariableDialog::name() const
{
  return m_nameEdit->text();
}

/// Gets the type of the variable
QString DFGNewVariableDialog::dataType() const
{
  return m_dataTypeEdit->text();
}

/// Gets the extension of the variable
QString DFGNewVariableDialog::extension() const
{
  return m_extensionEdit->text();
}
