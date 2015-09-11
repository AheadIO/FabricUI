// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGGetStringDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGGetStringDialog::DFGGetStringDialog(QWidget * parent, QString text, const DFGConfig & dfgConfig)
: DFGBaseDialog(parent, false, dfgConfig)
{
  m_lineEdit = new QLineEdit(text);
  m_lineEdit->selectAll();
  addInput(m_lineEdit);
}

DFGGetStringDialog::~DFGGetStringDialog()
{
}

// Allows only alpha-numeric text only 
void DFGGetStringDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("^[a-zA-Z0-9]*$*"));
}

// Filters the QLineEdit text with the regexFilter
void DFGGetStringDialog::setRegexFilter(QString regexFilter) {
  if(m_lineEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_lineEdit->setValidator(validator);
  }
}

QString DFGGetStringDialog::text() const
{
  return m_lineEdit->text();
}

