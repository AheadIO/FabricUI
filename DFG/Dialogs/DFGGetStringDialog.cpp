// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGGetStringDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

/// Constructor
DFGGetStringDialog::DFGGetStringDialog(QWidget * parent, QString text, const DFGConfig & dfgConfig, bool setAlphaNum)
: DFGBaseDialog(parent, false, dfgConfig)
{
  m_lineEdit = new QLineEdit(text);
  m_lineEdit->selectAll();
  addInput(m_lineEdit);

  // [Julien] Allows only alpha-numeric text only 
  // We do this because the nodes's name must be alpha-numerical only
  // and not contains "-, +, ?,"
  if(setAlphaNum) alphaNumicStringOnly();
}

/// Destructor
DFGGetStringDialog::~DFGGetStringDialog()
{
}

/// Allows only alpha-numeric text only 
void DFGGetStringDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("^[a-zA-Z0-9]*$*"));
}

/// Filters the QLineEdit text with the regexFilter
void DFGGetStringDialog::setRegexFilter(QString regexFilter) {
  if(m_lineEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_lineEdit->setValidator(validator);
  }
}

/// Gets the text
QString DFGGetStringDialog::text() const
{
  return m_lineEdit->text();
}

